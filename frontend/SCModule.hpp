#ifndef _SCMODULE_HPP
#define _SCMODULE_HPP

#include <string>
#include <vector>

#include "llvm/GlobalVariable.h"
#include "llvm/Function.h"

#include "SCPort.hpp"

using namespace std;
using namespace llvm;

struct SCPort;

struct SCModule {
private:
  string name;
  vector<Function* > threads;
  vector<SCPort*> ports;
  vector<GlobalVariable* > sharedVariables;
  
public:
  SCModule(string moduleName);
  
  vector<Function*>* getThreads();
  void addThread(Function* mainFct);
  
  vector<SCPort*>* getPorts();
  void addPort(SCPort* port);
  
  vector<GlobalVariable*>* getSharedVariables();
  void addSharedVariable(GlobalVariable* gv);
};

#endif
