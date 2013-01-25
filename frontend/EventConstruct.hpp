#ifndef _EVENTCONSTRUCT_HPP
#define _EVENTCONSTRUCT_HPP

#include "SCConstruct.hpp"

struct Event;

struct EventConstruct:public SCConstruct {
protected:
	Event * event;
	llvm::Value * missingEvent;
public:
	EventConstruct(llvm::Value* missingE);
	EventConstruct(Event * e);
	void setWaitedEvent(Event * e);
	llvm::Value* getMissingEvent();
	Event *getWaitedEvent();
	std::string toString();
};

#endif
