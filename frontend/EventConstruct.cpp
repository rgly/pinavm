#include "EventConstruct.hpp"

using namespace llvm;

EventConstruct::EventConstruct(SCEvent* e)
{
  this->event = e;
}

void
EventConstruct::setWaitedEvent(SCEvent* e)
{
  this->event = e;
}

SCEvent*
EventConstruct::getWaitedEvent()
{
  return this->event;
}

string
EventConstruct::toString()
{
  return "WAIT(event)";
}
