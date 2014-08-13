/**
 * BasicIOInliner.cpp (formerly TLMBasicPassImpl.cpp)
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
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstIterator.h>
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

#include "basic.h"
#include "bus.h"

#include "TwetoPass.h"
#include "TwetoPassImpl.h"

extern const std::string wFunName;
extern const std::string rFunName;

static sc_core::sc_module* getTargetModule(sc_core::sc_module* initiatorMod,
                                           basic::addr_t min, basic::addr_t max,
					   basic::addr_t* offset);

// =============================================================================
// inlineBasicIO
// 
// Replace indirect calls to write() or read() by direct calls 
// in the given LLVM bitcode function.
// =============================================================================
void TwetoPassImpl::inlineBasicIO(sc_core::sc_module* initiatorMod,
		                  llvm::Function* procf)
{

	std::string procfName = procf->getName();
	MSG("      Replace in the process's function : " + procfName +
	    "\n");

	std::ostringstream oss;
	std::vector < CallInfo * >*work = new std::vector < CallInfo * >;

	if (procf->isDeclaration())
		return;

	// do value analysis on the function
	se = &parent->getAnalysis<ScalarEvolution>(*procf);

	inst_iterator ii;
	for (ii = inst_begin(procf); ii != inst_end(procf); ii++) {
		Instruction & i = *ii;
		CallSite cs(&i);
		if (!cs.getInstruction())
			continue;
		// Candidate for a replacement
		Function *oldfun = cs.getCalledFunction();
		if (!oldfun || oldfun->isDeclaration())
			continue;
		std::string name = oldfun->getName();
		// === Write ===
		if (!strcmp(name.c_str(), wFunName.c_str())) {

			CallInfo *info = new CallInfo();
			info->oldcall = dyn_cast<CallInst>(cs.getInstruction());
			MSG("       Checking adress : ");
			info->addrArg = cs.getArgument(1);
			llvm::SCEV const *se_value = se->getSCEV (info->addrArg);
			llvm::ConstantRange value_range = se->getUnsignedRange
				(se_value);
			basic::addr_t const admax = ~(static_cast<basic::addr_t>(0));
			basic::addr_t min = value_range.getUnsignedMin()
						.getLimitedValue(admax),
			              max = value_range.getUnsignedMax()
						.getLimitedValue(admax);
			oss.str("");
			oss << std::hex << min << " to 0x" << max;
			MSG("0x" + oss.str() + "\n");

			// Retreive the target module using the address
			sc_core::sc_module* targetMod =
			    getTargetModule(initiatorMod, min, max, &info->offset);

			// can't optimize if the target component isn't predictible
			if (!targetMod)
				continue;

			// Save informations to build a new call later
			FunctionType *writeFunType =
			    this->basicWriteFun->getFunctionType();
			info->targetType =
			    writeFunType->getParamType(0);
			info->targetMod =
			     dynamic_cast<basic::target_module_base*>(targetMod);
			info->dataArg = cs.getArgument(2);
			info->isWrite = true;
			work->push_back(info);
		} else
			// === Read ===
		if (!strcmp(name.c_str(), rFunName.c_str())) {

			// Not yet supported

		}

	}

	// Before
	//procf->dump();

	// Replace calls
	std::vector < CallInfo * >::iterator it;
	for (it = work->begin(); it != work->end(); ++it) {
		CallInfo *i = *it;

		LLVMContext & context = getGlobalContext();
		FunctionType *writeFunType =
		    this->writeFun->getFunctionType();
		// Get a pointer to the target module
		IRBuilder<> irb(context);
		irb.SetInsertPoint (i->oldcall);
		Value* modPtr = createRelocatablePointer
			(writeFunType->getParamType(0), i->targetMod, &irb);

		// Compute the relative address in the target socket's POV
		Value* addr = irb.CreateSub (i->addrArg, ConstantInt::getSigned
				(Type::getInt32Ty(context), i->offset));

		// Create the new call
		Value *args[] = { modPtr, addr, i->dataArg };
		i->newcall =
		    CallInst::Create(this->writeFun,
				     ArrayRef < Value * >(args, 3));

		// Replace the old call
		BasicBlock::iterator it(i->oldcall);
		ReplaceInstWithInst(i->oldcall->getParent()->getInstList(),
				    it, i->newcall);
		i->oldcall->replaceAllUsesWith(i->newcall);

		// Inline the new call
		DataLayout *td = new DataLayout(this->llvmMod);
		InlineFunctionInfo ifi(NULL, td);
		bool success = InlineFunction(i->newcall, ifi);
		if (!success) {
			MSG("       The call cannot be inlined (it's not an error :D)");
		}

		MSG("       Call optimized (^_-)\n");
		callOptCounter++;
	}

	//std::cout << "==================================\n";
	// Run preloaded passes on the function to propagate constants
	funPassManager->run(*procf);
	// After
	// procf->dump();        
	// Check if the function is corrupt
	verifyFunction(*procf);
}

// =============================================================================
// getTargetModule
// 
// Retreive a module that is in the range of the given address.
// Of course, this module and the initiator module (arg) must be 
// connected to the same bus.
// =============================================================================
static sc_core::sc_module* getTargetModule(sc_core::sc_module* initiatorMod,
                                           basic::addr_t min, basic::addr_t max,
					   basic::addr_t* offset)
{

	std::vector < sc_core::sc_port_base * >*ports =
	    initiatorMod->m_port_vec;
	std::vector < sc_core::sc_port_base * >::iterator it;
	for (it = ports->begin(); it < ports->end(); ++it) {
		sc_core::sc_port_base * initiator = *it;
		sc_core::sc_interface * initiatorItf =
		    initiator->get_interface();
		std::string initiatorName = initiator->name();
		if (initiatorName.find("basic::initiator_socket") ==
		    std::string::npos)
			continue;
		basic::target_socket_base < true > *tsb =
		    dynamic_cast < basic::target_socket_base <
		    true > *>(initiatorItf);
		if (!tsb)
			continue;
		Bus *b = dynamic_cast < Bus * >(tsb->get_parent_object());
		if (!b)
			continue;
		basic::compatible_socket * target =
			b->getUniqueTarget(min, max, offset);
		if (!target)
			continue;

		std::string targetName = target->name();
		//MSG(" = "+initiatorName+" -> "+targetName+"\n");

		sc_core::sc_object * o = target->get_parent();
		sc_core::sc_module * targetMod =
		    dynamic_cast < sc_core::sc_module * >(o);
		return targetMod;
	}
	return NULL;
}

