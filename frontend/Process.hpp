#ifndef _PROCESS_HPP
#define _PROCESS_HPP

#include <string>
#include <vector>

#include "llvm/Function.h"
#include "Event.hpp"
#include "IRModule.hpp"

using namespace llvm;
using namespace std;

struct Event;
struct IRModule;

struct Process {
protected:
  string processName;
  IRModule* module;
  string fctName;
  Function* mainFct;
  std::vector<Event*> events;

public:
  Process(IRModule* mod, Function* fct, string name, string fctName);
  string getName();
  string getFctName();
  Function* getMainFct();
  IRModule* getModule();
  void addEvent(Event* ev);

};

#endif
