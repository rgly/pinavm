#ifndef _IRMODULE_HPP
#define _IRMODULE_HPP

#include <string>
#include <vector>

#include "llvm/GlobalVariable.h"
#include "llvm/Function.h"

#include "Port.hpp"
#include "Event.hpp"
#include "Process.hpp"

using namespace std;
using namespace llvm;

struct Port;
struct Event;
struct Process;

struct IRModule {
private:
  string name;
  string typeName;
  vector<Process*> processes;
  vector<Port*> ports;
  vector<Event*> events;

  vector<GlobalVariable*> sharedVariables;
  
public:
  IRModule(string typeName, string moduleName);
  
  vector<Process*>* getProcesses();
  void addProcess(Process* process);
  
  vector<Port*>* getPorts();
  void addPort(Port* port);

  vector<Event*>* getEvents();
  void addEvent(Event* event);
  
  vector<GlobalVariable*>* getSharedVariables();
  void addSharedVariable(GlobalVariable* gv);

  string getModuleType();
  string getUniqueName();
};

#endif
