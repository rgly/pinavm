#include <systemc>

#include "llvm/Intrinsics.h"
#include "llvm/Pass.h"
#include "llvm/Function.h"
#include "llvm/Module.h"
#include "llvm/CallingConv.h"
#include "llvm/DerivedTypes.h"
#include "llvm/InstrTypes.h"
#include "llvm/Instructions.h"
#include "llvm/Transforms/Utils/BasicInliner.h"

#include "Frontend.hpp"

#include "SCJit.hpp"
#include "SCCFactory.hpp"
#include "SCElab.h"

#include "config.h"

#include <iostream>
#include <typeinfo>

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

void Frontend::printElab(string prefix)
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

	for (itP = procs->begin(); itP < procs->end(); itP++) {
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
	TRACE_1("Getting ELAB\n");

	this->elab = new SCElab(llvmMod);
	this->elab->complete();

	this->scjit = new SCJit(llvmMod, elab);
	this->sccfactory = new SCCFactory(scjit);

// 	TRACE_1("Dumping typeSymbolTable\n");
// 	this->llvmMod->getTypeSymbolTable().dump();

	TRACE_1("Analyzing code\n");

	// Walk through call graph and build intermediate representation
	vector < Process * >::iterator processIt = this->elab->getProcesses()->begin();
	vector < Process * >::iterator endIt = this->elab->getProcesses()->end();
	std::vector < Function * >*fctStack = new std::vector < Function * >();

	if (processIt == endIt) {
		TRACE_1("WARNING: No process found.\n");
	}
	if (this->inlineFunctions) {
								
		for (; processIt < endIt; processIt++) {
			Process *proc = *processIt;
			Function *F = proc->getMainFct();
			TRACE_3("Parsing Function : " << F->getNameStr() << "\n");
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
						currentInst->dump();
						TRACE_4("Call not handled : " << calledFunction->getNameStr() << "\n");
						TRACE_4("Inlining function : " << calledFunction->getNameStr() << "\n");
						if (isInvoke)
							llvm::InlineFunction(dyn_cast<InvokeInst>(currentInst));
						else
							llvm::InlineFunction(dyn_cast<CallInst>(currentInst));
						// InlineFunction invalidates iterators => restart loop.
						goto start_for;
					}
					i++;
				}
			}
		}
	}
	
	processIt = this->elab->getProcesses()->begin();	
	fctStack = new std::vector < Function * >();

	for (; processIt < endIt; processIt++) {
		Process *proc = *processIt;
		fctStack->push_back(proc->getMainFct());
		proc->addUsedFunction(proc->getMainFct());
		this->scjit->setCurrentProcess(proc);
		TRACE_2("Process:" << proc->getName());
		while (!fctStack->empty()) {
			Function* F = fctStack->back();
			fctStack->pop_back();
			TRACE_3("Parsing Function : " << F->getNameStr() << "\n");
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
						currentInst->dump();
						TRACE_4("Call not handled : " << calledFunction->getNameStr() << "\n");
						fctStack->push_back(calledFunction);
						proc->addUsedFunction(calledFunction);
						
					} else if (calledFunction->getIntrinsicID() != Intrinsic::not_intrinsic) {
						TRACE_6("Encountered call to intrinsic function \"" << calledFunction->getNameStr() << "\" (id = " << calledFunction->getIntrinsicID() << "). Not parsing it.\n");
					}
					if (! callB) {
						fillGlobalVars(&*i);
					}
					i++;
				}
			}
		}
	}
	

	delete fctStack;
	return false;
}

Module *Frontend::getLLVMModule()
{
	return this->llvmMod;
}
