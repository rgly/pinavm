#include "SCModule.hpp"

using namespace llvm;

SCModule::SCModule(string moduleName)
{
  this->name = moduleName;
}

/*** Threads ***/
list<Function*>*
SCModule::getThreads()
{
  return & this->threads;
}

void
SCModule::addThread(Function* mainFct)
{
  this->threads.push_back(mainFct);
}


/*** Ports ***/
list<SCPort*>*
SCModule::getPorts()
{
  return & this->ports;
}

void
SCModule::addPort(SCPort* port)
{
  this->ports.push_back(port);
}


/*** Shared Variables ***/
list<GlobalVariable*>*
SCModule::getSharedVariables()
{
  return & this->sharedVariables;
}

void
SCModule::addSharedVariable(GlobalVariable gv)
{
  this->sharedVariables.push_back(gv);
}
