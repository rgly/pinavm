#include <systemc>

#include "llvm/Pass.h"
#include "llvm/Function.h"
#include "llvm/Module.h"
#include "llvm/CallingConv.h"
#include "llvm/DerivedTypes.h"
#include "llvm/InstrTypes.h"
#include "llvm/Instructions.h"

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
}

Frontend::~Frontend()
{
	if (this->sccfactory)
		delete this->sccfactory;

	if (this->scjit) {
		this->scjit->doFinalization();
		delete this->scjit;
	}

	if (this->elab)
		delete this->elab;

}

SCElab *Frontend::getElab()
{
	return this->elab;
}

SCCFactory *Frontend::getConstructs()
{
	return this->sccfactory;
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

	for (; processIt < endIt; processIt++) {
		Process *proc = *processIt;
		fctStack->push_back(proc->getMainFct());
		proc->addUsedFunction(proc->getMainFct());
		this->scjit->setCurrentProcess(proc);
		while (!fctStack->empty()) {
			Function *F = fctStack->back();
			fctStack->pop_back();
			TRACE_3("Parsing Function : " << F->getNameStr() << "\n");
			for (Function::iterator bb = F->begin(), be = F->end(); bb != be; ++bb) {
				BasicBlock::iterator i = bb->begin(), ie = bb->end();
				while (i != ie) {
					CallInst *callInst = dyn_cast < CallInst > (&*i);
					if (callInst) {
						if (! sccfactory->handle(F, &*bb, callInst)) {
							TRACE_6("CallInst : " << callInst << "\n");
							TRACE_6("CalledFct : " << callInst->getCalledFunction() << "\n");
							callInst->dump();
							TRACE_4("Call not handled : " << callInst->getCalledFunction()->getNameStr() << "\n");
							fctStack->push_back(callInst->getCalledFunction());
							proc->addUsedFunction(F);
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
