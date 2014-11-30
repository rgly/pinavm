#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Transforms/Utils/Cloning.h"

#include "Frontend.hpp"

#include "SCJit.hpp"
#include "SCCFactory.hpp"
#include "SCElab.h"

#include "config.h"

Frontend::Frontend(Module * M)
{
	this->llvmMod = M;
	this->inlineFunctions = false;
}

Frontend::~Frontend()
{
	delete this->sccfactory;

	if (this->scjit) {
		this->scjit->doFinalization();
		delete this->scjit;
	}

	delete this->elab;

}

SCJit *Frontend::getJit()
{
	return this->scjit;
}

SCElab *Frontend::getElab()
{
	return this->elab;
}

SCCFactory *Frontend::getConstructs()
{
	return this->sccfactory;
}

void Frontend::setInlineFunctions(bool b)
{
	this->inlineFunctions = b;
}

void Frontend::printElab(std::string prefix)
{
	TRACE("############### Printing ELAB ###############\n");
	this->elab->printElab(0, prefix);
	TRACE("\n");
}

void Frontend::printIR()
{
	std::vector < Process * >*procs = this->elab->getProcesses();
	std::vector < Process * >::iterator itP;

	TRACE("############### Printing IR ###############\n");

	for (itP = procs->begin(); itP < procs->end(); ++itP) {
		Process *p = *itP;
		TRACE("************ IR for process " << p <<
		      "*************\n");
		p->printIR(this->sccfactory);
	}
	TRACE("\n");
}

void
Frontend::fillGlobalVars(Instruction* inst)
{
	User::op_iterator opit = inst->op_begin();
	User::op_iterator opend = inst->op_end();
	for (; opit != opend; ++opit) {
		Value* op = *opit;
		if (isa<GlobalVariable>(op)) {
			GlobalValue* opcast = dyn_cast<GlobalVariable>(op);
			this->elab->addGlobalVariable(opcast);
		}
	}
}

bool Frontend::inlineForProcessFct(Function* F) {
	for(inst_iterator i = inst_begin(F); i != inst_end(F); ++i) {
		Function* calledFunction = NULL;
		Instruction* currentInst = &*i;
		BasicBlock* bb = currentInst->getParent();
		bool isInvoke = false;
		if (CallInst *callInst = dyn_cast < CallInst > (currentInst)) {
			calledFunction =  callInst->getCalledFunction();
		} else if(InvokeInst *invokeInst = dyn_cast < InvokeInst > (currentInst)) {
			calledFunction =  invokeInst->getCalledFunction();
			isInvoke = true;
		} 

		if (! calledFunction) {
			TRACE_6("Encountered call to function pointer. Not parsing it.\n");
		} else if (! this->sccfactory->handlerExists(F, bb, currentInst, calledFunction)) {
			TRACE_6("CallInst : " << currentInst << "\n");
			TRACE_6("CalledFct : " << calledFunction << "\n");
			PRINT_6(currentInst->dump());
			TRACE_4("Call not handled : " << calledFunction->getName().str() << "\n");
			TRACE_4("Inlining function : " << calledFunction->getName().str() << "\n");
			bool isInlined = false;
			llvm::InlineFunctionInfo ifi;
			if (isInvoke) {
				isInlined = llvm::InlineFunction(dyn_cast<InvokeInst>(currentInst), ifi);
			} else {
				isInlined = llvm::InlineFunction(dyn_cast<CallInst>(currentInst), ifi);
			}
			// InlineFunction invalidates iterators => restart loop.
			if (isInlined) {
				return true;
			}
		}
	}

	return false;
}

void Frontend::inlineProcessFct()
{
	for (Process *proc : *(this->elab->getProcesses()) ) {
		Function *F = proc->getMainFct();
		TRACE_3("Parsing Function : " << F->getName().str() << "\n");

		bool restartInlining = true;
		while(restartInlining) {
			restartInlining = this->inlineForProcessFct(F);
		}
	}
}

void Frontend::handleFctConstruct_and_GV(Process* proc, Function* F,
					std::vector<Function*>* fctStack)
{
	for(inst_iterator i = inst_begin(F); i != inst_end(F); ++i) {
		bool callB = false;
		Function* calledFunction = NULL;
		Instruction* currentInst = &*i;
		BasicBlock* bb = currentInst->getParent();
		if (CallInst *callInst = dyn_cast < CallInst > (currentInst)) {
			callB = true;
			calledFunction =  callInst->getCalledFunction();
		} else if(InvokeInst *invokeInst = dyn_cast < InvokeInst > (currentInst)) {
			callB = true;
			calledFunction =  invokeInst->getCalledFunction();
		}

		if (! calledFunction) {
			TRACE_6("Encountered call to function pointer. Not parsing it.\n");
		} else if (! this->sccfactory->handle(proc, F, bb, currentInst, calledFunction)) {
			TRACE_6("CallInst : " << currentInst << "\n");
			TRACE_6("CalledFct : " << calledFunction << "\n");
			PRINT_6(currentInst->dump());
			TRACE_4("Call not handled : " << calledFunction->getName().str() << "\n");
			fctStack->push_back(calledFunction);
			proc->addUsedFunction(calledFunction);

		} else if (calledFunction->getIntrinsicID() != Intrinsic::not_intrinsic) {
			TRACE_6("Encountered call to intrinsic function \"" << calledFunction->getName().str() << "\" (id = " << calledFunction->getIntrinsicID() << "). Not parsing it.\n");
		}
		if (! callB) {
			this->fillGlobalVars(currentInst);
		}
	}
}

void Frontend::handleConstruct_and_GV()
{
	// Walk through call graph and build intermediate representation
	std::vector < Function * >*fctStack = new std::vector < Function * >();

	for (Process *proc : *(this->elab->getProcesses()) ) {
		fctStack->push_back(proc->getMainFct());
		proc->addUsedFunction(proc->getMainFct());
		this->scjit->setCurrentProcess(proc);
		TRACE_2("Process:" << proc->getName() << "\n");
		while (!fctStack->empty()) {
			Function* F = fctStack->back();
			fctStack->pop_back();
			TRACE_3("Parsing Function : " << F->getName().str() << "\n");
			PRINT_3(F->dump());
			this->handleFctConstruct_and_GV(proc, F, fctStack);
		}
	}
	delete fctStack;
}

bool Frontend::run()
{
	TRACE_1("Getting ELAB\n");

	this->elab = new SCElab(llvmMod);
	this->elab->complete();

	this->scjit = new SCJit(llvmMod, elab);
	this->sccfactory = new SCCFactory(scjit);


	TRACE_1("Analyzing code\n");

	if (this->elab->getProcesses()->empty()) {
		TRACE_1("WARNING: No process found.\n");
	}

	TRACE_4("NB of functions BEFORE inlining : " << llvmMod->size() << "\n");

	if (this->inlineFunctions) {
		this->inlineProcessFct();
	}
	
	TRACE_4("NB of functions after inlining : " << llvmMod->size() << "\n");

	this->handleConstruct_and_GV();

	TRACE_4("NB of functions after frontend : " << llvmMod->size() << "\n");

	return false;
}

Module *Frontend::getLLVMModule()
{
	return this->llvmMod;
}
