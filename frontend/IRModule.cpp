#include "IRModule.hpp"
#include "Process.hpp"
#include "Port.hpp"

#include "config.h"

#include "SCElab.h"
#include "sysc/kernel/sc_module.h"

using namespace llvm;

IRModule::IRModule(const SCElab* el, std::string moduleType, std::string moduleName)
	: ElabMember(el)
{
	this->typeName = moduleType;
	this->name = moduleName;
}

/******** Threads ********/
std::vector < Process * >*IRModule::getProcesses()
{
	return &this->processes;
}

void IRModule::addProcess(Process * process)
{
	this->processes.push_back(process);
}


/******** Ports ********/
std::vector < Port * >*IRModule::getPorts()
{
	return &this->ports;
}

void IRModule::addPort(Port * port)
{
	this->ports.push_back(port);
}

/******** Events ********/
std::vector < Event * >*IRModule::getEvents()
{
	return &this->events;
}

void IRModule::addEvent(Event * event)
{
	this->events.push_back(event);
}


/******** Shared Variables ********/
std::vector < GlobalVariable * >*IRModule::getSharedVariables()
{
	return &this->sharedVariables;
}

void IRModule::addSharedVariable(GlobalVariable * gv)
{
	this->sharedVariables.push_back(gv);
}

std::string IRModule::getModuleType()
{
	return this->typeName;
}

std::string IRModule::getUniqueName()
{
	return this->name;
}

/********** Pretty print **********/
void IRModule::printElab(int sep, std::string prefix)
{
	std::vector < Process * >::iterator itProcesses;
	std::vector < Port * >::iterator itPorts;
	this->printPrefix(sep, prefix);
	TRACE("Module : " << this->name << "\n");
	auto sub_mods = this->getChildIRMod();

	for (auto sub_mod : sub_mods) {
		auto sub_name = sub_mod->getUniqueName();
		this->printPrefix(sep+3, prefix);
		TRACE(sub_name << "\n");
	}

	for (itPorts = this->ports.begin(); itPorts < this->ports.end(); ++itPorts) {
		Port* port = *itPorts;
		port->printElab(sep + 3, prefix);
	}
	TRACE("\n");
	for (itProcesses = this->processes.begin(); itProcesses < this->processes.end(); ++itProcesses) {
		Process *p = *itProcesses;
		p->printElab(sep + 3, prefix);
	}
}

std::vector<IRModule*> IRModule::getChildIRMod()
{
	SCElab* elab = const_cast<SCElab*>(this->getElab());
	assert(elab);
	std::vector<IRModule*> sub_irmods;
	auto* sc_mod = elab->getSCModule(this);
	assert(sc_mod);

	auto& sub_mods = sc_mod->get_child_objects();
	for (auto* sub_obj : sub_mods) {
		auto* sub_mod = dynamic_cast<sc_core::sc_module*>(sub_obj);
		if (sub_mod) {
			IRModule* sub_irmod = elab->getIRModule(sub_mod);
			assert(sub_irmod);
			sub_irmods.push_back(sub_irmod);
		}
	}
	return sub_irmods;
}
