#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "EventConstruct.hpp"

using namespace llvm;

EventConstruct::EventConstruct(Event* e)
{
  this->event = e;
}

void
EventConstruct::setWaitedEvent(Event* e)
{
  this->event = e;
}

Event*
EventConstruct::getWaitedEvent()
{
  return this->event;
}

string
EventConstruct::toString()
{
  return "WAIT(event)";
}
