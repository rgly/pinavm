#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "Event.hpp"

using namespace llvm;

Event::Event(string event) 
{
  this->eventName = event;
}

string
Event::toString()
{
  return this->eventName;
}
