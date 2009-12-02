#ifndef _NOTIFYCONSTRUCT_HPP
#define _NOTIFYCONSTRUCT_HPP

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstruct.hpp"
#include "Event.hpp"

using namespace llvm;

struct Event;

struct NotifyConstruct:public SCConstruct {

	protected:Event * event;
      public:
	NotifyConstruct(Event * e);
	void setNotifiedEvent(Event * e);
	Event *getNotifiedEvent();
	string toString();
};

#endif
