#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstruct.hpp"
#include "NotifyConstruct.hpp"

using namespace llvm;

NotifyConstruct::NotifyConstruct(Event* e)
{
  this->event = e;
}

void
NotifyConstruct::setNotifiedEvent(Event* e)
{
  this->event = e;
}

Event*
NotifyConstruct::getNotifiedEvent()
{
  return this->event;
}

char*
NotifyConstruct::toString()
{
  return "NOTIFY(event)";
}
