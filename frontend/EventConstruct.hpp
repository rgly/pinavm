#ifndef _EVENTCONSTRUCT_HPP
#define _EVENTCONSTRUCT_HPP

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstruct.hpp"

using namespace llvm;

struct Event;

struct EventConstruct:public SCConstruct {
	protected:Event * event;
      public:
	EventConstruct(Value* missingValue);
	EventConstruct(Event * e);
	void setWaitedEvent(Event * e);
	Event *getWaitedEvent();
	std::string toString();
};

#endif
