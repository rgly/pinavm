#include <cxxabi.h>

#include <llvm/Type.h>

#include "IRModule.hpp"
#include "Process.hpp"
#include "Port.hpp"
#include "Event.hpp"
#include "FUtils.hpp"
#include "Channel.hpp"
#include "SimpleChannel.hpp"
#include "ClockChannel.hpp"
#include "BasicChannel.hpp"

#include "llvm/LLVMContext.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Assembly/Writer.h"
#include "llvm/InstrTypes.h"


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

#include "basic.h"
#include "bus.h"

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
	this->busMap.clear();
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
				std::pair < sc_core::sc_module *,
				IRModule * >(mod, m));
	this->ir2scModules.insert(this->ir2scModules.end(), std::pair <IRModule *, sc_core::sc_module *>(m, mod));
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
		<< " ; Fonction : " << mainFctName << " " << static_cast<void *>(mainFct) << " mainFct->name = " << mainFct->getName().str() << " type arg 1 : " << cast<PointerType>(mainFct->arg_begin()->getType())->getElementType()->getStructName().str() << "\n");

	mod->addProcess(p);
	this->processes.push_back(p);
	this->processMap.insert(this->processMap.end(), std::pair < sc_core::sc_process_b *,	Process * >(process, p));

	return p;
}

Port * SCElab::trySc_Signal(IRModule * mod, 
                            sc_core::sc_interface * itf,
                            std::string &itfTypeName,
                            sc_core::sc_port_base * port, 
                            std::string portName         ) {
	Port * theNewPort = NULL;
	Channel* ch;
	std::string match = "N7sc_core9sc_signalI";
	char temp[10];
	std::string variableTypeName("");


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
			itfType = this->llvmMod->getTypeByName(StringRef(itfTypeName));

			if (itfType) {
				TRACE_4("Type found !\n");
			} else {
				itfType = Type::getInt32Ty(getGlobalContext());
				TRACE_4("SCElab.addPort() -> interface type not found -> consider enum (32 bits integer) " << variableTypeName << "\n");
			}
		}
		TRACE_4("typeName of variable accessed through port : " << variableTypeName << "\n");
		TRACE_4("type of variable accessed through port : " << itfType << "\n");

		theNewPort = new Port(mod, portName, port);
		std::map < sc_core::sc_interface*, Channel * >::iterator itM;
		if ((itM = this->channelsMap.find(itf)) == this->channelsMap.end()) {
			ch = new SimpleChannel((Type*) itfType, variableTypeName);
			this->channels.push_back(ch);
			this->channelsMap.insert(this->channelsMap.end(), std::pair < sc_core::sc_interface *, Channel * >(itf, ch));
            TRACE_4("New channel !\n");
		} else {
			ch = itM->second;
		}

		TRACE_2("Add (sc_port_base) " << port << " -> (SIMPLE_PORT) " << portName << " with channel " << ch << "\n");
		TRACE_4("Channel contains type: "<< ch->getType() << "\n");

		theNewPort->addChannel(ch);
	}
	return theNewPort;
}



Port * SCElab::trySc_Clock(IRModule * mod, 
			 sc_core::sc_interface* itf, std::string &itfTypeName,
			 sc_core::sc_port_base * port, std::string portName) {
	Port * theNewPort = NULL;
	std::string match = "N7sc_core8sc_clockE";
	if (itfTypeName.find(match) == 0) {

		theNewPort = new Port(mod, portName, port);
		Channel* ch;
		std::map < sc_core::sc_interface*, Channel * >::iterator itM;
		if ((itM = this->channelsMap.find(itf)) == this->channelsMap.end()) {
			ch = new ClockChannel();
			this->channelsMap.insert(this->channelsMap.end(), std::pair < sc_core::sc_interface *, Channel * >(itf, ch));
        } else {
			ch = itM->second;
		}
		theNewPort->addChannel(ch);

		// Read sc_clock information
		// NOTE : clockchannel is not contained in SCElab.channels.
                sc_core::sc_clock* sc_cch =
				 dynamic_cast<sc_core::sc_clock*>(itf) ;
                ClockChannel* cch = dynamic_cast<ClockChannel*>(ch) ;
                cch->setClock(  sc_cch->period().to_seconds(),
                                sc_cch->duty_cycle(),
                                sc_cch->start_time().to_seconds(),
                                sc_cch->posedge_first() ) ;

		TRACE_2("Add (sc_port_base) " << port << " -> (CLOCK_PORT) " << theNewPort << " with channel " << ch <<"\n");
	}
	return theNewPort;
}

Port * SCElab::tryBasicTarget(IRModule * mod, 
                              sc_core::sc_interface* itf,
                              std::string &itfTypeName,
                              sc_core::sc_port_base * port,
                              std::string portName) {
	Port * theNewPort = NULL;
	// basic::target_socket<Bus, true>
	std::string match1 = "N5basic13target_socketI3BusLb1EEE";
	// basic::target_socket<target, false>
	std::string match2 = "N5basic13target_socketI6targetLb0EEE";

	// TODO: too fuzzy
	match1 = "N5basic13target_socket";
	match2 = "N5basic19target_socket_falseE";
	
	if ((itfTypeName.find(match1) == 0) ||
	    (itfTypeName.find(match2) == 0)) {
		theNewPort = new Port(mod, portName, port);

		std::map < sc_core::sc_interface*, Channel * >::iterator itM;
		Channel* ch;
		// initiator socket bound to target socket on
		// the bus.
		basic::target_socket_base<true> *tp =
			dynamic_cast<basic::target_socket_base<true> *>(itf);
		if (tp) {
			Bus *bb =
				dynamic_cast<Bus *>(tp->get_parent());
			ASSERT(bb);
			if ((itM = this->channelsMap.find(bb)) == this->channelsMap.end()) {
				ch = new BasicChannel(bb->name());
				this->channelsMap.insert(this->channelsMap.end(),std::pair < sc_core::sc_interface *, Channel * >(bb, ch));
				this->busMap.insert(this->busMap.end(),std::pair <Channel * , Bus *>(ch, bb));
				TRACE_2("BasicChannel bus name " << bb->name() << "\n");
			} else {
				ch = itM->second;
			}
			theNewPort->addChannel(ch);
			TRACE_2("Add (sc_port_base) " << port
				<< " -> (BASIC_TARGET_SOCKET) " << theNewPort 
				<< " with channel " << ch <<"\n");
		} else {
			TRACE_2("Examined the initiator socket on the basic Bus. Did nothing.\n");
		}
	}
	return theNewPort;
}

template<typename T>
static void try_type (sc_core::sc_interface* itf, 
			      const char *&res,
			      const char *type) {
	T *tmp = dynamic_cast<T *>(itf);
	if (tmp) {
		TRACE_5("Got channel of type " << type << "\n");
		res = tmp->name();
	}
}

std::string SCElab::getBasicChannelName(sc_core::sc_interface* itf) {

	const char *res = NULL;

	try_type<basic::target_socket_base<true>     >(itf, res, "basic::target_socket_base<true>");
	try_type<basic::target_socket_base<false>    >(itf, res, "basic::target_socket_base<false>");
	try_type<basic::initiator_socket_base<true>  >(itf, res, "basic::initiator_socket_base<true>");
	try_type<basic::initiator_socket_base<false> >(itf, res, "basic::initiator_socket_base<false>");

	if (res) {
		return res;
	} else {
		TRACE_1("WARNING: Unknown kind of channel (typeid is " << typeid(*itf).name() << ")\n");
		return "Unknown";
	}
}
				   

Port * SCElab::tryBasicInitiator(IRModule * mod, 
				 sc_core::sc_interface* itf, std::string &itfTypeName,
				 sc_core::sc_port_base * port, std::string portName) {
	Port * theNewPort = NULL;
	// basic::initiator_socket<Bus, true>
	//string match1 = "N5basic16initiator_socketI3BusLb1EEE";
	// TODO: match is too fuzzy 
	std::string match1 = "N5basic16initiator_socketI";

	// basic::initiator_socket<initiator, false>
	//string match2 = "N5basic16initiator_socketI9initiatorLb0EEE";
	// TODO: match is too fuzzy 
	std::string match2 = "N5basic16initiator_socketI";

	// TODO: debug
	match1 = "N5basic21initiator_socket_trueE";
	// TODO: match is a bit too fuzzy.
	match2 = "N5basic16initiator_socket";

	if ((itfTypeName.find(match1) == 0) ||
	    (itfTypeName.find(match2) == 0)) {
		theNewPort = new Port(mod, portName, port);

		std::map < sc_core::sc_interface*, Channel * >::iterator itM;
		BasicChannel* ch;
		basic::initiator_socket_base<true> *tp =
			dynamic_cast<basic::initiator_socket_base<true> *>(itf);
		if (tp) {
			Bus *bb =
				dynamic_cast<Bus *>(tp->get_parent_object());
			ASSERT(bb);
			if ((itM = this->channelsMap.find(bb)) == this->channelsMap.end()) {
				ch = new BasicChannel(bb->name());
				this->channelsMap.insert(this->channelsMap.end(),
							 std::pair < sc_core::sc_interface *, Channel * >(bb, ch));
                this->busMap.insert(this->busMap.end(),
                            std::pair <Channel * , Bus *>(ch, bb));
				TRACE_2("BasicChannel bus name " << bb->name() << "\n");
			} else {
				ch = dynamic_cast<BasicChannel *>(itM->second);
				ASSERT(ch);
				TRACE_2("Reusing BasicChannel " << ch->getChannelName() << "\n");
			}
			theNewPort->addChannel(ch);
			TRACE_2("Add (sc_port_base) " << port 
				<< " -> (BASIC_INITIATOR_SOCKET) " << theNewPort 
				<< " with channel " << ch << "\n");
		} else {
			TRACE_2("Examined the target socket on the basic Bus. Did nothing.\n");
		}
	}
	return theNewPort;
}



Port *SCElab::addPort(IRModule * mod, sc_core::sc_port_base * port)
{
	std::string match;

	Port* theNewPort = NULL;
	std::map < sc_core::sc_port_base *, Port * >::iterator it;

	if ((it = this->portsMap.find(port)) == this->portsMap.end()) {

		std::stringstream ss;
		ss << mod->getUniqueName() << "_" << static_cast<void *>(port);
		std::string portName = ss.str();

		int nbItfs = ((sc_core::sc_port_b<int>*) port)->size();
		TRACE_6("port concerned: " << portName << " Nb_itfs = " << nbItfs << "\n");
		TRACE_6("SystemC port name: " << port->name() << "\n");


		sc_core::sc_interface* itf = port->get_interface();
//	sc_core::sc_port_b<bool>* pb = (sc_core::sc_port_b<bool>*) port;
		if (port==NULL) {
			TRACE_6("TESTING IF PORT = NULL ************************************************************************");
		}
		if (itf==NULL) {
			TRACE_6("TESTING IF Interface is NULL ****************************************************************** ");
			//TRACE_6(port->getName());
		}
//	const char* typeName = typeid(*(pb->m_interface)).name();
		const char* typeName = typeid(*itf).name();
//		N7sc_core5sc_inIbEE

		std::string itfTypeName(typeName);

		TRACE_4("m_interface of port is: " << itfTypeName 
			<< " (" << abi::__cxa_demangle(itfTypeName.c_str(), NULL, NULL, NULL) << ")\n");

		/* take the first match */
		if (theNewPort == NULL)
			theNewPort = trySc_Signal(mod, itf, itfTypeName, port, portName);

		if (theNewPort == NULL)
			theNewPort = trySc_Clock(mod, itf, itfTypeName, port, portName);

		if (theNewPort == NULL)
			theNewPort = tryBasicTarget(mod, itf, itfTypeName, port, portName);

		if (theNewPort == NULL)
			theNewPort = tryBasicInitiator(mod, itf, itfTypeName, port, portName);

		if (theNewPort == NULL) {
			ABORT("Could not analyze port " << portName << " with interface type " << itfTypeName);
		}
		mod->addPort(theNewPort);
		this->ports.push_back(theNewPort);
		this->portsMap.insert(this->portsMap.end(), std::pair < sc_core::sc_port_base *, Port * >(port, theNewPort));
	}  else {
		theNewPort = it->second;
	}

	return theNewPort;
}

Event *SCElab::addEvent(Process * process, sc_core::sc_event * event)
{
	std::map < sc_core::sc_event *, Event * >::iterator it;
	Event *e;
	static int counter=0;
	IRModule *mod = process->getModule();
	TRACE_3("Look for event : " << event << "\n");
	if ((it = this->eventsMap.find(event)) == this->eventsMap.end()) {
		TRACE_3("NOT Found !\n");
		char suffix[10];
		sprintf(suffix, "%d", ++counter );
		std::string eventName = mod->getUniqueName() + "_event_" + suffix;
		e = new Event(eventName);
		this->events.push_back(e);
		this->eventsMap[event] = e;
	} else {
		TRACE_3("Found !\n");
		e = it->second;
	}
	process->addEvent(e);
	e->addProcess(process);
	TRACE_2("Add (sc_event)     " << event << " -> (Event) " << e << " (" << e->getEventName() << ") to (Process) " << process << "\n");
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
	for (itM = this->modules.begin(); itM < this->modules.end(); ++itM) {
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

Bus *SCElab::getBus(Channel *chan) 
{
    return this->busMap.find(chan)->second;
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
	std::vector < const sc_core::sc_event * >::iterator it;
	for (it = eventsVector.begin(); it < eventsVector.end(); ++it) {
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
	std::vector < sc_core::sc_module * >modules =
		sc_core::sc_get_curr_simcontext()->get_module_registry()->m_module_vec;
	std::vector < sc_core::sc_module * >::iterator modIt;
	for (modIt = modules.begin(); modIt < modules.end(); ++modIt) {
		sc_core::sc_module * mod = *modIt;
		IRModule *m = this->addModule(mod);
		std::vector < sc_core::sc_port_base * >*ports = mod->m_port_vec;
		std::vector < sc_core::sc_port_base * >::iterator it;
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

// The Reason I do not get Sensitive List while initializing Ports
// is that Processes are initialized after Ports, so getting Process
// List is impossible when construct Port.
std::vector < Process*>* SCElab::getProcessOfPort(sc_core::sc_port_base* scport, bool IsThread)
{
	std::vector<Process*>* static_thread_of_port = new std::vector<Process*>;
	std::vector<sc_core::sc_bind_ef*> sc_processes  ;

	if (IsThread) {
		sc_processes = scport->m_bind_info->thread_vec ;
	} else {
		sc_processes = scport->m_bind_info->method_vec ;
	}


        sc_core::sc_process_b* temp_sc_process;
        Process* temp_process;
        for (unsigned int i = 0 ; i < sc_processes.size() ; ++i) {
		temp_sc_process = (sc_processes[i])->handle ;
		temp_process = this->processMap[ temp_sc_process ] ;
		static_thread_of_port->push_back(temp_process) ;
	}


	return static_thread_of_port ;
}
