#include <llvm/TypeSymbolTable.h>
#include <llvm/Type.h>

#include "IRModule.hpp"
#include "Process.hpp"
#include "Port.hpp"
#include "Event.hpp"
#include "FUtils.hpp"
#include "Channel.hpp"
#include "SimpleChannel.hpp"
#include "ClockChannel.hpp"

#include "llvm/LLVMContext.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Assembly/Writer.h"


#include "sysc/kernel/sc_process_table.h"
#include "sysc/kernel/sc_simcontext.h"
#include "sysc/kernel/sc_thread_process.h"
#include "sysc/kernel/sc_module_registry.h"
#include "sysc/kernel/sc_module.h"
#include "sysc/kernel/sc_name_gen.h"
#include "sysc/kernel/sc_process.h"
#include "sysc/kernel/sc_event.h"
#include "sysc/communication/sc_port.h"
#include "sysc/communication/sc_bind_ef.h"
#include "sysc/communication/sc_bind_info.h"
#include "sysc/kernel/sc_process_handle.h"

#include "SCElab.h"
#include "config.h"

SCElab::SCElab(Module * llvmModule)
{
	this->llvmMod = llvmModule;
}

SCElab::~SCElab()
{
	this->modulesMap.clear();
	this->processMap.clear();
	this->eventsMap.clear();
	this->portsMap.clear();
	this->ir2scModules.clear();

	FUtils::deleteVector < IRModule * >(&this->modules);
	FUtils::deleteVector < Process * >(&this->processes);
	FUtils::deleteVector < Event * >(&this->events);
	FUtils::deleteVector < Port * >(&this->ports);
}

IRModule *SCElab::addModule(sc_core::sc_module * mod)
{
	const char *moduleType = typeid(*mod).name();
	std::string moduleName = (std::string) mod->name();
	IRModule *m = new IRModule(moduleType, moduleName);
	this->modules.push_back(m);
	this->modulesMap.insert(this->modulesMap.end(),
				pair < sc_core::sc_module *,
				IRModule * >(mod, m));
	this->ir2scModules.insert(this->ir2scModules.end(), pair <IRModule *, sc_core::sc_module *>(m, mod));
	TRACE_2("Added (sc_module)  " << (void *) mod << " -> (IRModule) "
		<< m << " with name " << moduleName << "\n");
	return m;
}

Process *SCElab::addProcess(IRModule * mod,
			    sc_core::sc_process_b * process)
{
	std::string fctName(process->func_process);
	std::string modType = mod->getModuleType();
	std::string moduleName = mod->getUniqueName();
	std::string mainFctName = "_ZN" + modType + utostr(fctName.size()) + fctName + "Ev";
	std::string processName = moduleName + "_" + mainFctName;
	Function *mainFct = this->llvmMod->getFunction(mainFctName);

//   Function* fct;
//   const std::vector<const Type*> argsType;
//   FunctionType* FT = FunctionType::get(Type::getVoidTy(getGlobalContext()), argsType, false);
	Process *p = new Process(mod, mainFct, processName, mainFctName);
//	mainFct->dump();
	TRACE_2("Add (sc_process_b) " << process << " -> (Process) " << p
		<< " ; Fonction : " << mainFctName << " " << mainFct << " mainFct->name = " << mainFct->getNameStr() << " type arg 1 : " << this->llvmMod->getTypeName(cast<PointerType>(mainFct->arg_begin()->getType())->getElementType()) << "\n");

	mod->addProcess(p);
	this->processes.push_back(p);
	this->processMap.insert(this->processMap.end(), pair < sc_core::sc_process_b *,	Process * >(process, p));

	return p;
}

Port *SCElab::addPort(IRModule * mod, sc_core::sc_port_base * port)
{
	std::string match;
	char buffer[10];
	char temp[10];

	Port* theNewPort;
	std::map < sc_core::sc_port_base *, Port * >::iterator it;

	if ((it = this->portsMap.find(port)) == this->portsMap.end()) {

		sprintf(buffer, "%lx", (unsigned long) port);
		string portName = mod->getUniqueName() + "_0x" + buffer;

		sc_core::sc_interface* itf = port->get_interface();
//	sc_core::sc_port_b<bool>* pb = (sc_core::sc_port_b<bool>*) port;

//	const char* typeName = typeid(*(pb->m_interface)).name();
		const char* typeName = typeid(*itf).name();
//		N7sc_core5sc_inIbEE

		std::string itfTypeName(typeName);
		std::string variableTypeName("");
		Channel* ch;
		std::map < sc_core::sc_interface*, Channel * >::iterator itM;

		TRACE_4("m_interface of port is: " << itfTypeName << "\n");
		sprintf(temp, "%d", (int) itfTypeName.size());
		TRACE_4("Found : " << temp << "\n");

		match = "N7sc_core9sc_signalI";
		if (itfTypeName.find(match) == 0) {

			size_t found = itfTypeName.find_first_of("E");
			sprintf(temp, "%d", (int) found);
			TRACE_4("Found : " << temp << "\n");
			sprintf(temp, "%d", (int) match.size());
			TRACE_4("match size : " << temp << "\n");
			size_t typeLength = found - match.size();
			sprintf(temp, "%d", (int) typeLength);
			TRACE_4("typeLength : " << temp << "\n");
			variableTypeName = itfTypeName.substr(match.size(), typeLength);

			const Type* itfType = NULL;
			if (variableTypeName == "b") {
				itfType = Type::getInt1Ty(getGlobalContext());
				if (itfType)
					TRACE_4("Boolean type found !\n");
			} else if(variableTypeName == "i") {
				itfType = Type::getInt32Ty(getGlobalContext());
				if (itfType)
					TRACE_4("Integer type found !\n");
			} else if(variableTypeName == "N5sc_dt7sc_uintILi8") {
				itfType = Type::getInt32Ty(getGlobalContext());
				if (itfType)
					TRACE_4("Unsigned integer type found !\n");
			} else {
				itfType = this->llvmMod->getTypeSymbolTable().lookup(itfTypeName);

				if (itfType) {
					TRACE_4("Type found !\n");
				} else {
					itfType = Type::getInt32Ty(getGlobalContext());
					TRACE_4("SCElab.addPort() -> interface type not found -> consider enum (32 bits integer) " << variableTypeName << "\n");
				}
			}
			TRACE_4("typeName of variable accessed through port : " << variableTypeName << "\n");
			TRACE_4("type of variable accessed through port : " << itfType << "\n");
			theNewPort = new Port(mod, portName);
			if ((itM = this->channelsMap.find(itf)) == this->channelsMap.end()) {
				ch = new SimpleChannel((Type*) itfType, itfTypeName);
				this->channels.push_back(ch);
				this->channelsMap.insert(this->channelsMap.end(), pair < sc_core::sc_interface *, Channel * >(itf, ch));

				TRACE_4("New channel !\n");
			} else {
				ch = itM->second;
			}

			TRACE_2("Add (sc_port_base) " << port << " -> (SIMPLE_PORT) " << theNewPort << " with channel " << ch << "\n");
			theNewPort->addChannel(ch);
		}
		match = "N7sc_core8sc_clockE";
		if (itfTypeName.find(match) == 0) {
			theNewPort = new Port(mod, portName);
			if ((itM = this->channelsMap.find(itf)) == this->channelsMap.end()) {
				ch = new ClockChannel();
				this->channelsMap.insert(this->channelsMap.end(), pair < sc_core::sc_interface *, Channel * >(itf, ch));

			} else {
				ch = itM->second;
			}
			theNewPort->addChannel(ch);
			TRACE_2("Add (sc_port_base) " << port << " -> (CLOCK_PORT) " << theNewPort << " with channel " << ch <<"\n");
		}
		mod->addPort(theNewPort);
		this->ports.push_back(theNewPort);
		this->portsMap.insert(this->portsMap.end(), pair < sc_core::sc_port_base *, Port * >(port, theNewPort));

	}  else {
		theNewPort = it->second;
	}

	return theNewPort;
}

Event *SCElab::addEvent(Process * process, sc_core::sc_event * event)
{
	std::map < sc_core::sc_event *, Event * >::iterator it;
	Event *e;
	IRModule *mod = process->getModule();
	if ((it = this->eventsMap.find(event)) == this->eventsMap.end()) {
		char buffer[10];
		sprintf(buffer, "%lx", (unsigned long) event);
		string eventName = mod->getUniqueName() + "_0x" + buffer;
		e = new Event(eventName);
		this->events.push_back(e);
		this->eventsMap[event] = e;
	} else {
		e = it->second;
	}
	process->addEvent(e);
	e->addProcess(process);
	TRACE_2("Add (sc_event)     " << event << " -> (Event) " << e << " (" << e->toString() << ") to (Process) " << process << "\n");
	return e;

}

IRModule *SCElab::getIRModule(void *moduleAddr)
{
	return this->modulesMap.find((sc_core::sc_module *) moduleAddr)->
	    second;
}

Process *SCElab::getProcess(void *processAddr)
{
	return this->processMap.find((sc_core::sc_process_b *) processAddr)->second;
}

Port *SCElab::getPort(void *portAddr)
{
	return this->portsMap.find((sc_core::sc_port_base *) portAddr)->second;
}

Event *SCElab::getEvent(void *eventAddr)
{
	return this->eventsMap.find((sc_core::sc_event *) eventAddr)->second;
}

void SCElab::printElab(int sep, std::string prefix)
{
	std::vector < IRModule * >::iterator itM;
	this->printPrefix(sep, prefix);
	for (itM = this->modules.begin(); itM < this->modules.end(); itM++) {
		IRModule *m = *itM;
		m->printElab(sep, prefix);
	}
}


int
SCElab::getNumProcesses()
{
	return this->processes.size();
}

int
SCElab::getNumEvents()
{
	return this->events.size();
}

std::vector < Process * >*
SCElab::getProcesses()
{
	return &this->processes;
}

std::vector < Event * >*
SCElab::getEvents()
{
	return &this->events;
}

std::vector < Port *>*
SCElab::getPorts()
{
	return &this->ports;
}

std::vector < Channel* >*
SCElab::getChannels()
{
	return &this->channels;
}

sc_core::sc_module * SCElab::getSCModule(IRModule * irmod)
{
	return this->ir2scModules.find(irmod)->second;
}

void SCElab::addGlobalVariable(GlobalValue * globalVar)
{
	if (find(globalVariables.begin(), globalVariables.end(), globalVar) == globalVariables.end())
		this->globalVariables.push_back(globalVar);
}

std::vector < GlobalValue * >* SCElab::getGlobalVariables()
{
	return & this->globalVariables;
}

void
SCElab::addProcessAndEvents(sc_core::sc_process_b *theProcess, sc_core::sc_module * mod)
{
	IRModule *m = this->getIRModule(mod);
	Process *process = this->addProcess(m, theProcess);
	
	std::vector < const sc_core::sc_event * >eventsVector = theProcess->m_static_events;
	vector < const sc_core::sc_event * >::iterator it;
	for (it = eventsVector.begin(); it < eventsVector.end(); it++) {
		sc_core::sc_event * ev = (sc_core::sc_event *) * it;
		this->addEvent(process, ev);
	}
}


void
SCElab::complete()
{
	sc_core::sc_thread_handle thread_p;
	sc_core::sc_method_handle method_p;

	// MM: why do this here? stopping the execution of the
	// elaboration after calling end_of_elaboration seems a much
	// better idea than calling it ourselves.
	// To call the "end_of_elaboration()" methods.
//	sc_core::sc_get_curr_simcontext()->initialize(true);

	//------- Get modules and ports --------
	vector < sc_core::sc_module * >modules =
		sc_core::sc_get_curr_simcontext()->get_module_registry()->m_module_vec;
	vector < sc_core::sc_module * >::iterator modIt;
	for (modIt = modules.begin(); modIt < modules.end(); ++modIt) {
		sc_core::sc_module * mod = *modIt;
		IRModule *m = this->addModule(mod);
		std::vector < sc_core::sc_port_base * >*ports = mod->m_port_vec;
		vector < sc_core::sc_port_base * >::iterator it;
		for (it = ports->begin(); it < ports->end(); ++it) {
			sc_core::sc_port_base * p = *it;
			this->addPort(m, p);
		}
	}

	//------- Get processes and events --------
	sc_core::sc_process_table * processes = sc_core::sc_get_curr_simcontext()->m_process_table;
	for (thread_p = processes->thread_q_head(); thread_p; thread_p = thread_p->next_exist()) {
		sc_core::sc_process_b * theProcess = thread_p;
		sc_core::sc_module * mod = (sc_core::sc_module *) thread_p->m_semantics_host_p;
		addProcessAndEvents(theProcess, mod);
	}

	for (method_p = processes->method_q_head(); method_p; method_p = method_p->next_exist()) {
		sc_core::sc_method_process* theP = method_p;
		sc_core::sc_module * mod = (sc_core::sc_module *) theP->m_semantics_host_p;
		addProcessAndEvents(theP, mod);
	}
}
