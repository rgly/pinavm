#include "Process.hpp"

Process::Process(IRModule* mod, Function* mainFunc, string name, string funcName)
{
  this->module = mod;
  this->mainFct = mainFunc;
  this->processName = name;
  this->fctName = funcName;
}

string
Process::getFctName()
{
  return this->fctName;
}

string
Process::getName()
{
  return this->processName;
}

Function*
Process::getMainFct()
{
  return this->mainFct;
}

IRModule*
Process::getModule()
{
  return this->module;
}

void
Process::addEvent(Event* ev)
{
  this->events.push_back(ev);
}
