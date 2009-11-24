#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "Event.hpp"

using namespace llvm;

Event::Event(Process* p, string event) 
{
  this->process = p;
  this->eventName = event;
}

string
Event::toString()
{
  return this->eventName;
}

Process*
Event::getProcess()
{
  return this->process;
}
