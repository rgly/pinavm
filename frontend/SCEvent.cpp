#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCEvent.hpp"

using namespace llvm;

SCEvent::Event(string event) 
{
  this->eventName = event;
}

string
SCEvent::toString()
{
  return this->eventName;
}
