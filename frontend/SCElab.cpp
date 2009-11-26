#include "IRModule.hpp"
#include "Process.hpp"
#include "Port.hpp"
#include "Event.hpp"
#include "FUtils.hpp"

#include "SCElab.h"
#include "config.h"

SCElab::SCElab(Module* llvmModule)
{
  this->llvmMod = llvmModule;
}

SCElab::~SCElab()
{
  this->modulesMap.clear();
  this->processMap.clear();
  this->eventsMap.clear();
  this->portsMap.clear();
  this->sc2irModules.clear();

  FUtils::deleteVector<IRModule*>(& this->modules);
  FUtils::deleteVector<Process*>(& this->processes);
  FUtils::deleteVector<Event*>(& this->events);
  FUtils::deleteVector<Port*>(& this->ports);
}

IRModule*
SCElab::addModule(sc_core::sc_module* mod)
{
  const char* moduleType = typeid(*mod).name();
  string moduleName = (string) mod->name();
  IRModule* m = new IRModule(moduleType, moduleName);
  this->modules.push_back(m);
  this->modulesMap.insert(this->modulesMap.end(), pair<sc_core::sc_module*, IRModule*>(mod, m));
  this->sc2irModules.insert(this->sc2irModules.end(), pair<IRModule*, sc_core::sc_module*>(m, mod));
  TRACE_2("Added (sc_module)  " << (void*) mod << " -> (IRModule) " << m << " with name " << moduleName << "\n");
  return m;
}

Process*
SCElab::addProcess(IRModule* mod, sc_core::sc_process_b* process)
{
  const char* fctName = process->func_process;
  string modType = mod->getModuleType();
  string moduleName = mod->getUniqueName();
  string mainFctName =  "_ZN" + (string) modType + "6" + (string) fctName + "Ev";
  string processName = moduleName + "::" + mainFctName;
  Function* mainFct = this->llvmMod->getFunction(mainFctName);

  Process* p = new Process(mod, mainFct, processName, mainFctName);
  TRACE_2("Add (sc_process_b) " << process << " -> (Process) " << p << "\n");

  mod->addProcess(p);
  this->processes.push_back(p);
  this->processMap.insert(this->processMap.end(), pair<sc_core::sc_process_b*, Process*>(process, p));

  return p;
}

Port*
SCElab::addPort(IRModule* mod, sc_core::sc_port_base* port)
{
  char buffer[10];
  sprintf(buffer, "%x", (int) port);
  string portName = mod->getUniqueName() + "::0x" + buffer;
  //  sc_core::sc_interface* if = port->get_interface();

  Port* p = new Port(mod, portName);
  TRACE_2("Add (sc_port_base) " << port << " -> (Port) " << p << "\n");

  this->ports.push_back(p);
  this->portsMap.insert(this->portsMap.end(), pair<sc_core::sc_port_base*, Port*>(port, p));
  return p;
}

Event*
SCElab::addEvent(Process* process, sc_core::sc_event* event)
{
  std::map<sc_core::sc_event*, Event*>::iterator it;
  Event* e;
  IRModule* mod = process->getModule();
  if ((it = this->eventsMap.find(event)) == this->eventsMap.end()) {
    char buffer[10];
    sprintf(buffer, "%x", (int) event);
    string eventName = mod->getUniqueName() + "::0x" + buffer;
    e = new Event(process, eventName);
    this->events.push_back(e);
    this->eventsMap[event] = e;
  } else {
    e = it->second;
  }
  process->addEvent(e);
  TRACE_2("Add (sc_event)     " << event << " -> (Event) " << e << " to (Process) " << process <<"\n");
  return e;

}

IRModule*
SCElab::getIRModule(void* moduleAddr)
{
  return this->modulesMap.find((sc_core::sc_module*) moduleAddr)->second;
}

Process*
SCElab::getProcess(void* processAddr)
{
  return this->processMap.find((sc_core::sc_process_b*) processAddr)->second;
}

Port*
SCElab::getPort(void* portAddr)
{
  return this->portsMap.find((sc_core::sc_port_base*) portAddr)->second;
}

Event*
SCElab::getEvent(void* eventAddr)
{
  return this->eventsMap.find((sc_core::sc_event*) eventAddr)->second;
}

void
SCElab::printElab(int sep, string prefix)
{
  std::vector<IRModule*>::iterator itM;
  this->printPrefix(sep, prefix);
  for (itM = this->modules.begin() ; itM < this->modules.end() ; itM++) {
    IRModule* m = *itM;
    m->printElab(sep, prefix);
  }  
}


std::vector<Process*>*
SCElab::getProcesses()
{
  return &this->processes;
}




sc_core::sc_module*
SCElab::getSCModule(IRModule* irmod)
{
  return this->sc2irModules.find(irmod)->second;
}
