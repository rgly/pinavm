#include "IRModule.hpp"

IRModule::IRModule(string moduleType, string moduleName)
{
  this->typeName = moduleType;
  this->name = moduleName;
}

/******** Threads ********/
vector<Process*>*
IRModule::getProcesses()
{
  return & this->processes;
}

void
IRModule::addProcess(Process* process)
{
  this->processes.push_back(process);
}


/******** Ports ********/
vector<Port*>*
IRModule::getPorts()
{
  return & this->ports;
}

void
IRModule::addPort(Port* port)
{
  this->ports.push_back(port);
}

/******** Events ********/
vector<Event*>*
IRModule::getEvents()
{
  return & this->events;
}

void
IRModule::addEvent(Event* event)
{
  this->events.push_back(event);
}


/******** Shared Variables ********/
vector<GlobalVariable*>*
IRModule::getSharedVariables()
{
  return & this->sharedVariables;
}

void
IRModule::addSharedVariable(GlobalVariable* gv)
{
  this->sharedVariables.push_back(gv);
}

string
IRModule::getModuleType()
{
  return this->typeName;
}

string
IRModule::getUniqueName()
{
  return this->name;
}
