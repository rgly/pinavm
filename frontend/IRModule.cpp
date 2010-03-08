#include "IRModule.hpp"
#include "Process.hpp"
#include "Port.hpp"
#include "Event.hpp"

using namespace std;
using namespace llvm;

IRModule::IRModule(string moduleType, string moduleName)
{
	this->typeName = moduleType;
	this->name = moduleName;
}

/******** Threads ********/
vector < Process * >*IRModule::getProcesses()
{
	return &this->processes;
}

void IRModule::addProcess(Process * process)
{
	this->processes.push_back(process);
}


/******** Ports ********/
vector < Port * >*IRModule::getPorts()
{
	return &this->ports;
}

void IRModule::addPort(Port * port)
{
	this->ports.push_back(port);
}

/******** Events ********/
vector < Event * >*IRModule::getEvents()
{
	return &this->events;
}

void IRModule::addEvent(Event * event)
{
	this->events.push_back(event);
}


/******** Shared Variables ********/
vector < GlobalVariable * >*IRModule::getSharedVariables()
{
	return &this->sharedVariables;
}

void IRModule::addSharedVariable(GlobalVariable * gv)
{
	this->sharedVariables.push_back(gv);
}

string IRModule::getModuleType()
{
	return this->typeName;
}

string IRModule::getUniqueName()
{
	return this->name;
}

/********** Pretty print **********/
void IRModule::printElab(int sep, string prefix)
{
	std::vector < Process * >::iterator itProcesses;
	std::vector < Port * >::iterator itPorts;
	this->printPrefix(sep, prefix);
	TRACE("Module : " << this << "\n");
	for (itPorts = this->ports.begin(); itPorts < this->ports.end(); ++itPorts) {
		Port* port = *itPorts;
		port->printElab(sep + 3, prefix);
	}
	TRACE("\n");
	for (itProcesses = this->processes.begin(); itProcesses < this->processes.end();
	     itProcesses++) {
		Process *p = *itProcesses;
		p->printElab(sep + 3, prefix);
	}
}
