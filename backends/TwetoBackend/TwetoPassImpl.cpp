/**
 * TwetoPassImpl.cpp
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui, Guillaume Sergent
 * @contact : si-mohamed.lamraoui@imag.fr, guillaume.sergent@ens-lyon.fr
 * @date : 2011/03/17
 * @copyright : Verimag 2011, 2014
 */

#include <iostream>
#include <string>
#include <fstream>
#include <map>

#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/Mangler.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Verifier.h"
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/InstIterator.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Pass.h"
#include "llvm/PassManagers.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/Function.h"

#include "Port.hpp"
#include "Channel.hpp"
#include "SimpleChannel.hpp"
#include "SCElab.h"
#include "Process.hpp"
#include "IRModule.hpp"
#include "SCJit.hpp"
#include "ALLConstruct.h"
#include "SCCFactory.hpp"

#include "LinkExternalBitcode.h"

#include "sysc/kernel/sc_process_table.h"
#include "sysc/kernel/sc_simcontext.h"
#include "sysc/kernel/sc_thread_process.h"
#include "sysc/kernel/sc_module_registry.h"
#include "sysc/kernel/sc_module.h"
#include "sysc/kernel/sc_process.h"
#include "sysc/communication/sc_port.h"
#include "sysc/pinavm/permalloc.h"

#include "TwetoPassImpl.h"

int proc_counter = 0;

extern const std::string wFunName =
    "_ZN5basic21initiator_socket_baseILb0EE5writeEjji";
extern const std::string rFunName =
    "_ZN5basic21initiator_socket_baseILb0EE4readEjRji";

extern GlobalVariable* sbase;

// =============================================================================
// TwetoPassImpl 
// 
// Constructor
// 
// =============================================================================
TwetoPassImpl::TwetoPassImpl(Frontend * fe, ExecutionEngine * ee,
			     enum tweto_opt_level optimize, bool disableMsg)
{
	this->callOptCounter = 0;
	this->rwCallsCounter = 0;
	this->fe = fe;
	this->engine = ee;
	this->optlevel = optimize;
	this->disableMsg = disableMsg;
	this->elab = fe->getElab();
	LLVMContext& c = getGlobalContext();
	// Checking machine's data size
	int n = sizeof(void *);
	if (n == 8)
		this->intptrType = Type::getInt64Ty(c);
	else			// n = 4
		this->intptrType = Type::getInt32Ty(c);
}


// =============================================================================
// runOnModule
// 
//  
// 
// =============================================================================
bool TwetoPassImpl::runOnModule(Module & M)
{

	MSG("\n============== Tweto Pass =============\n");
	this->llvmMod = &M;

	this->llvmMod = LinkExternalBitcode(this->llvmMod,
					    "backends/TwetoBackend/runtime_lib/tweto_call_method.bc");

	// Retrieve the method that does all the vtable calculations
	// in order to call the actual 'write' method (see replaceCallsInProcess)
	this->writeFun =
	    this->llvmMod->getFunction("tweto_call_write_method");
	if (!this->writeFun) {
		std::cerr << "tweto_call_write_method is missing,";
		std::cerr << "pass aborded!" << std::endl;
		return false;
	}
	// Retrieve the base write method in order to get the types
	// of the 'this' pointer, the address and the data
	// (should be always mangled like this, so if the Basic-protocol's includes
	//  are correctly imported, no error will be throw)
	this->basicWriteFun = this->llvmMod->getFunction(wFunName);
	if (!this->basicWriteFun) {
		std::cerr << "basic's write method is missing,";
		std::cerr << "pass aborded!" << std::endl;
		return false;
	}

	writeFun->dump();

	// Initialize function passes
	DataLayout *target = new DataLayout(this->llvmMod);
	funPassManager = new FunctionPassManager(this->llvmMod);
	funPassManager->add(target);
	funPassManager->add(createIndVarSimplifyPass());
	funPassManager->add(createLoopUnrollPass());
	funPassManager->add(createInstructionCombiningPass());
	funPassManager->add(createReassociatePass());
	funPassManager->add(createGVNPass());
	funPassManager->add(createCFGSimplificationPass());
	funPassManager->add(createConstantPropagationPass());

	// Modules
	std::vector < sc_core::sc_module * >modules =
	    sc_core::sc_get_curr_simcontext()->get_module_registry()->
	    m_module_vec;
	std::vector < sc_core::sc_module * >::iterator modIt;
	for (modIt = modules.begin(); modIt < modules.end(); ++modIt) {
		sc_core::sc_module * initiatorMod = *modIt;
		std::string moduleName =
		    (std::string) initiatorMod->name();
		// Optimize this module
		optimize(initiatorMod);
	}

	// Check if the module is corrupt
	verifyModule(*this->llvmMod);
	std::ostringstream oss;
	oss << callOptCounter;
	MSG("\n Pass report - " + oss.str() + "/" + "?");
	MSG(" - opt/total\n");
	MSG("===========================================\n\n");

	// TODO : handle false case
	return true;
}

// =============================================================================
// optimize
// 
// Optimize all processes of a given sc_module
// =============================================================================
void TwetoPassImpl::optimize(sc_core::sc_module * initiatorMod)
{
	// Looking for calls in process
	sc_core::sc_process_table * processes =
	    initiatorMod->sc_get_curr_simcontext()->m_process_table;
	sc_core::sc_thread_handle thread_p;
	for (thread_p = processes->thread_q_head();
	     thread_p; thread_p = thread_p->next_exist()) {
		sc_core::sc_process_b * proc = thread_p;
		if (proc->m_semantics_host_p != initiatorMod)
			continue;
		Function* f = findFunction (initiatorMod, proc);
		if (!f)
			continue;
		Function* f2 = andOOPIsGone (f, initiatorMod);
		proc->m_bc_semantics_p = f2;
	}
	sc_core::sc_method_handle method_p;
	for (method_p = processes->method_q_head();
	     method_p; method_p = method_p->next_exist()) {
		//sc_core::sc_method_process *proc = method_p;
		sc_core::sc_process_b * proc = method_p;
		if (proc->m_semantics_host_p != initiatorMod)
			continue;
		Function* f = findFunction (initiatorMod, proc);
		if (!f)
			continue;
		Function* f2 = andOOPIsGone (f, initiatorMod);
		proc->m_bc_semantics_p = f2;
	}
}

Function* TwetoPassImpl::findFunction (sc_core::sc_module* initiatorMod,
                                       sc_core::sc_process_b* proc)
{
	// compute the gnu-v3 mangled name of the function
	std::string fctName = proc->func_process;
	std::string modType = typeid(*initiatorMod).name();
	std::string mainFctName = "_ZN" + modType + 
	utostr(fctName.size()) + fctName + "Ev";
	// Gets the function by its name (dlsym equivalent)
	Function *f = this->llvmMod->getFunction(mainFctName);
	return f;
}

Value* TwetoPassImpl::createRelocatablePointer
	(Type* type, void* initiatorMod, IRBuilder<>* irb)
{
	if (optlevel == staticopt) {
		assert (permalloc::is_from (initiatorMod));
		ptrdiff_t this_offset = permalloc::get_offset (initiatorMod);
		Constant *offset = ConstantInt::getSigned(intptrType, this_offset);
		Value* sbase_val = irb->CreateLoad (sbase);
		Value* pi8thisAddr = irb->CreateGEP
			(sbase_val, std::vector<Value*>(1,offset));
		return irb->CreateBitCast (pi8thisAddr, type);
	} else {
		Constant *initiatorModVal = ConstantInt::getSigned(intptrType,
			      reinterpret_cast<intptr_t> (initiatorMod));
		return irb->CreateIntToPtr(initiatorModVal, type, "this_ptr");
	}
}

// =============================================================================
// andOOPIsGone (formerly: createProcess)
// 
// Formerly, OOP permitted the same SC_{METHOD,THREAD} functions to apply
// to each copy of a SC_MODULE. Aaaaand it's gone !
// (but OTOH we enable better optimizations)
// Creates a new C-style function that calls the old member function with the
// given sc_module. The call is then inlined.
// FIXME: assumes the method is non-virtual and that sc_module is the first
//        inherited class of the SC_MODULE
// =============================================================================
Function *TwetoPassImpl::andOOPIsGone(Function * oldProc,
				      sc_core::sc_module * initiatorMod)
{
	if (!oldProc)
		return NULL;

	// can't statically optimize if the address of the module isn't predictible
	// TODO: also handle already-static variables, which also have
	// fixed $pc-relative addresses
	if (staticopt == optlevel && !permalloc::is_from (initiatorMod))
		return NULL;

	LLVMContext & context = getGlobalContext();

	FunctionType *funType = oldProc->getFunctionType();
	Type *type = funType->getParamType(0);

	FunctionType *newProcType =
	    FunctionType::get(oldProc->getReturnType(),
			      ArrayRef < Type * >(), false);

	// Create the new function
	std::ostringstream id;
	id << proc_counter++;
	std::string name =
	    oldProc->getName().str() + std::string("_clone_") + id.str();
	Function *newProc =
	    Function::Create(newProcType, Function::ExternalLinkage, name,
			     this->llvmMod);
	assert(newProc->empty());
	newProc->addFnAttr(Attribute::InlineHint);

	// Create call to old function
	BasicBlock *bb = BasicBlock::Create(context, "entry", newProc);
	IRBuilder <> *irb = new IRBuilder <> (context);
	irb->SetInsertPoint(bb);

	Value* thisAddr = createRelocatablePointer (type, initiatorMod, irb);

	CallInst *ci = irb->CreateCall(oldProc,
				       ArrayRef < Value * >(std::vector<Value*>(1,thisAddr)));
	//bb->getInstList().insert(ci, thisAddr);
	if (ci->getType()->isVoidTy())
		irb->CreateRetVoid();
	else
		irb->CreateRet(ci);

	// The function should be valid now
	verifyFunction(*newProc);

	{			// Inline the call
		DataLayout *td = new DataLayout(this->llvmMod);
		InlineFunctionInfo i(NULL, td);
		bool success = InlineFunction(ci, i);
		assert(success);
		verifyFunction(*newProc);
	}

	// further optimize the function
	inlineBasicIO (initiatorMod, newProc);

	newProc->dump();
	return newProc;
}


// =============================================================================
// MSG
// 
// Print the given message 
// if the user did not use -dis-opt-msg
// =============================================================================
void TwetoPassImpl::MSG(std::string msg)
{
	if (!this->disableMsg)
		std::cout << msg;
}
