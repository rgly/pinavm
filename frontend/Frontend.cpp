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

#include "sysc/kernel/sc_process_table.h"
#include "sysc/kernel/sc_process_handle.h"
#include "sysc/kernel/sc_simcontext.h"
#include "sysc/kernel/sc_thread_process.h"
#include "sysc/kernel/sc_module_registry.h"

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
	sc_core::sc_thread_handle thread_p;	// Pointer to thread process accessing.

	TRACE_1("Getting ELAB\n");

	// To call the "end_of_elaboration()" methods.
	sc_core::sc_get_curr_simcontext()->initialize(true);

	this->elab = new SCElab(llvmMod);
	std::vector < Function * >*fctStack =
	    new std::vector < Function * >();

	//------- Get modules and ports --------
	vector < sc_core::sc_module * >modules = sc_core::sc_get_curr_simcontext()->get_module_registry()->m_module_vec;
	vector < sc_core::sc_module * >::iterator modIt;

	for (modIt = modules.begin(); modIt < modules.end(); ++modIt) {
		sc_core::sc_module * mod = *modIt;

		IRModule *m = this->elab->addModule(mod);
		std::vector < sc_core::sc_port_base * >*ports =
		    mod->m_port_vec;

		vector < sc_core::sc_port_base * >::iterator it;
		for (it = ports->begin(); it < ports->end(); ++it) {
			sc_core::sc_port_base * p = *it;
			this->elab->addPort(m, p);
		}
	}

	//------- Get processes and events --------
	sc_core::sc_process_table * processes = sc_core::sc_get_curr_simcontext()->m_process_table;
	for (thread_p = processes->thread_q_head(); thread_p;
	     thread_p = thread_p->next_exist()) {
		sc_core::sc_process_b * theProcess = thread_p;
		sc_core::sc_module * mod =
		    (sc_core::sc_module *) thread_p->m_semantics_host_p;
		IRModule *m = this->elab->getIRModule(mod);
		Process *process = this->elab->addProcess(m, theProcess);

		std::vector < const sc_core::sc_event * >eventsVector =
		    theProcess->m_static_events;
		vector < const sc_core::sc_event * >::iterator it;
		for (it = eventsVector.begin(); it < eventsVector.end();
		     it++) {
			sc_core::sc_event * ev =
			    (sc_core::sc_event *) * it;
			this->elab->addEvent(process, ev);
		}
	}

	this->scjit = new SCJit(llvmMod, elab);
	this->sccfactory = new SCCFactory(scjit);

	TRACE_1("Analyzing code\n");

	// Walk through call graph and build intermediate representation
	vector < Process * >::iterator processIt = this->elab->getProcesses()->begin();
	vector < Process * >::iterator endIt = this->elab->getProcesses()->end();

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
