#include "llvm/Intrinsics.h"
#include "llvm/Function.h"
#include "llvm/Module.h"
#include "llvm/Instructions.h"
#include "llvm/Transforms/Utils/Cloning.h"
//#include "llvm/Transforms/Utils/BasicInliner.h"

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

bool Frontend::run()
{
	bool isInlined;

	TRACE_1("Getting ELAB\n");

	this->elab = new SCElab(llvmMod);
	this->elab->complete();

	this->scjit = new SCJit(llvmMod, elab);
	this->sccfactory = new SCCFactory(scjit);


	TRACE_1("Analyzing code\n");

	// Walk through call graph and build intermediate representation
	std::vector < Process * >::iterator processIt = this->elab->getProcesses()->begin();
	std::vector < Process * >::iterator endIt = this->elab->getProcesses()->end();
	std::vector < Function * >*fctStack = new std::vector < Function * >();


	TRACE_4("NB of functions BEFORE inlining : " << llvmMod->size() << "\n");
	

	if (processIt == endIt) {
		TRACE_1("WARNING: No process found.\n");
	}
	if (this->inlineFunctions) {
								
		for (; processIt < endIt; ++processIt) {
			Process *proc = *processIt;
			Function *F = proc->getMainFct();
			TRACE_3("Parsing Function : " << F->getName().str() << "\n");

		start_for:
			for (Function::iterator bb = F->begin(), be = F->end(); bb != be; ++bb) {
				BasicBlock::iterator i = bb->begin(), ie = bb->end();
				while (i != ie) {
					Function* calledFunction = NULL;
					Instruction* currentInst = &*i;
					bool isInvoke = false;
					if (CallInst *callInst = dyn_cast < CallInst > (currentInst)) {
						calledFunction =  callInst->getCalledFunction();
					} else if(InvokeInst *invokeInst = dyn_cast < InvokeInst > (currentInst)) {
						calledFunction =  invokeInst->getCalledFunction();
						isInvoke = true;
					} 
					
					if (! calledFunction) {
						TRACE_6("Encountered call to function pointer. Not parsing it.\n");
					} else if (! this->sccfactory->handlerExists(F, &*bb, currentInst, calledFunction)) {
						TRACE_6("CallInst : " << currentInst << "\n");
						TRACE_6("CalledFct : " << calledFunction << "\n");
						PRINT_6(currentInst->dump());
						TRACE_4("Call not handled : " << calledFunction->getName().str() << "\n");
						TRACE_4("Inlining function : " << calledFunction->getName().str() << "\n");
						isInlined = false;
						llvm::InlineFunctionInfo ifi;
						if (isInvoke)
						  isInlined = llvm::InlineFunction(dyn_cast<InvokeInst>(currentInst), ifi);
						else
						  isInlined = llvm::InlineFunction(dyn_cast<CallInst>(currentInst), ifi);
						// InlineFunction invalidates iterators => restart loop.
						if (isInlined)
							goto start_for;
					}
					++i;
				}
			}
		}
	}
	
	TRACE_4("NB of functions after inlining : " << llvmMod->size() << "\n");

	processIt = this->elab->getProcesses()->begin();	
	fctStack = new std::vector < Function * >();

	for (; processIt < endIt; ++processIt) {
		Process *proc = *processIt;
		fctStack->push_back(proc->getMainFct());
		proc->addUsedFunction(proc->getMainFct());
		this->scjit->setCurrentProcess(proc);
		TRACE_2("Process:" << proc->getName() << "\n");
		while (!fctStack->empty()) {
			Function* F = fctStack->back();
			fctStack->pop_back();
			TRACE_3("Parsing Function : " << F->getName().str() << "\n");
			PRINT_3(F->dump());
			for (Function::iterator bb = F->begin(), be = F->end(); bb != be; ++bb) {
				BasicBlock::iterator i = bb->begin(), ie = bb->end();
				bool callB; 
				while (i != ie) {
					callB = false;
					Function* calledFunction = NULL;
					Instruction* currentInst = &*i;
					if (CallInst *callInst = dyn_cast < CallInst > (currentInst)) {
						callB = true;
						calledFunction =  callInst->getCalledFunction();
					} else if(InvokeInst *invokeInst = dyn_cast < InvokeInst > (currentInst)) {
						callB = true;
						calledFunction =  invokeInst->getCalledFunction();
					}
					
					if (! calledFunction) {
						TRACE_6("Encountered call to function pointer. Not parsing it.\n");
					} else if (! this->sccfactory->handle(proc, F, &*bb, currentInst, calledFunction)) {
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
						fillGlobalVars(&*i);
					}
					++i;
				}
			}
		}
	}

	TRACE_4("NB of functions after frontend: " << llvmMod->size() << "\n");

	delete fctStack;
	return false;
}

Module *Frontend::getLLVMModule()
{
	return this->llvmMod;
}
