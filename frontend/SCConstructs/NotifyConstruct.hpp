#ifndef _NOTIFYCONSTRUCT_HPP
#define _NOTIFYCONSTRUCT_HPP

#include "llvm/Value.h"

#include "SCConstruct.hpp"

using namespace llvm;

struct Event;

struct NotifyConstruct:public SCConstruct {

protected:
	Event * event;
	Value * missingEvent;
public:
	NotifyConstruct(Event * e);
	NotifyConstruct(Value * missingE);
	void setNotifiedEvent(Event * e);
	Event *getNotifiedEvent();
	std::string toString();
};

#endif
