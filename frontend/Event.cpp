#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "Event.hpp"

using namespace llvm;

Event::Event(char* event) 
{
  this->eventName = event;
}

char*
Event::toString()
{
  return this->eventName;
}
