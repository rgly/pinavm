#ifndef _SCMODULE_HPP
#define _SCMODULE_HPP

#include <string>
#include <list>

#include "llvm/GlobalVariable"

using namespace llvm;

struct SCModule {
private:
  string name;
  list<Function*> threads;
  list<Function*> ports;
  list<Function*> sharedVariables;

public:
  SCModule(string moduleName);

  list<Function*>* getThreads();
  void addThread(Function* mainFct);

  list<SCPort*>* getPorts();
  void addPort(SCPort* port);

  list<GlobalVariable*>* getSharedVariables();
  void addGlobalVariable(GlobalVariable* gv);
}

#endif
