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
	// MM: it's safe to delete a NULL pointer => the if()s are
	// MM: unnecessary.
	if (this->sccfactory)
		delete this->sccfactory;

	if (this->scjit) {
		this->scjit->doFinalization();
		delete this->scjit;
	}

	if (this->elab)
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
					CallInst *callInst = dyn_cast < CallInst > (&*i);
					if (callInst) {
						if (! this->sccfactory->handlerExists(F, &*bb, callInst)) {
							TRACE_6("CallInst : " << callInst << "\n");
							TRACE_6("CalledFct : " << callInst->getCalledFunction() << "\n");
							callInst->dump();
							TRACE_4("Call not handled : " << callInst->getCalledFunction()->getNameStr() << "\n");
							TRACE_4("Inlining function : " << callInst->getCalledFunction()->getNameStr() << "\n");
							llvm::InlineFunction(callInst);
							// InlineFunction invalidates iterators => restart loop.
							goto start_for;
						}
					}
					BasicBlock::iterator tmpend = bb->end();
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
			Function *F = fctStack->back();
			fctStack->pop_back();
			TRACE_3("Parsing Function : " << F->getNameStr() << "\n");
			for (Function::iterator bb = F->begin(), be = F->end(); bb != be; ++bb) {
				BasicBlock::iterator i = bb->begin(), ie = bb->end();
				while (i != ie) {
					CallInst *callInst = dyn_cast < CallInst > (&*i);
					if (callInst) {
						if (callInst->getCalledFunction()->getIntrinsicID() != Intrinsic::not_intrinsic) {
							TRACE_6("Encountered call to intrinsic function \"" << callInst->getCalledFunction()->getNameStr() << "\" (id = " << callInst->getCalledFunction()->getIntrinsicID() << "). Not parsing it.\n");
						} else if (! sccfactory->handle(proc, F, &*bb, callInst)) {
							TRACE_6("CallInst : " << callInst << "\n");
							TRACE_6("CalledFct : " << callInst->getCalledFunction() << "\n");
							callInst->dump();
							TRACE_4("Call not handled : " << callInst->getCalledFunction()->getNameStr() << "\n");
							fctStack->push_back(callInst->getCalledFunction());
							proc->addUsedFunction(callInst->getCalledFunction());
						}
					} else {
						fillGlobalVars(&*i);
					}
					BasicBlock::iterator tmpend = bb->end();
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
