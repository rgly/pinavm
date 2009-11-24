#ifndef _SCELAB_H
#define _SCELAB_H

#include <systemc>

#include <iterator>
#include <map>
#include <vector>

#include <string>
#include <sstream>

#include "IRModule.hpp"
#include "Process.hpp"
#include "Port.hpp"
#include "Event.hpp"
#include "FUtils.hpp"

#include "llvm/Module.h"

#include "sysc/kernel/sc_module.h"
#include "sysc/kernel/sc_name_gen.h"
#include "sysc/kernel/sc_process.h"
#include "sysc/kernel/sc_event.h"
#include "sysc/communication/sc_port.h"
#include "sysc/kernel/sc_process_handle.h"

#include "config.h"

class SCElab {
 private:
  std::map<sc_core::sc_module*, IRModule*> modulesMap;
  std::map<sc_core::sc_process_b*, Process*> processMap;
  std::map<sc_core::sc_port_base*, Port*> portsMap;
  std::map<sc_core::sc_event*, Event*> eventsMap;

  std::map<IRModule*, sc_core::sc_module*> sc2irModules;
  
  std::vector<IRModule*> modules;
  std::vector<Process*> processes;
  std::vector<Port*> ports;
  std::vector<Event*> events;
  
  llvm::Module* llvmMod;

 public:
  SCElab(Module* llvmModule); 
  ~SCElab();
   
  IRModule* addModule(sc_core::sc_module* mod);
  Process* addProcess(IRModule* mod, sc_core::sc_process_b* process);
  Port* addPort(IRModule* mod, sc_core::sc_port_base* port);
  Event* addEvent(Process* process, sc_core::sc_event* event);

  IRModule* getIRModule(void* moduleAddr);
  Process* getProcess(void* processAddr);
  Port* getPort(void* portAddr);
  Event* getEvent(void* eventAddr);

  sc_core::sc_module* getSCModule(IRModule* irmod);

  std::vector<Process*>* getProcesses();
};

#endif
