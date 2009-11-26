#ifndef _PROCESS_HPP
#define _PROCESS_HPP

#include <map>
#include <string>
#include <vector>


#include "llvm/InstrTypes.h"
#include "llvm/Instructions.h"
#include "llvm/BasicBlock.h"
#include "llvm/Function.h"

#include "config.h"

using namespace llvm;
using namespace std;

//struct Event;
struct IRModule;
struct Event;
struct SCCFactory;

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
  void printIR(SCCFactory* sccfactory);

};

#endif
