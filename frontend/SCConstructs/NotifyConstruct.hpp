#ifndef _NOTIFYCONSTRUCT_HPP
#define _NOTIFYCONSTRUCT_HPP

#include "llvm/IR/Value.h"

#include "SCConstruct.hpp"

using namespace llvm;

class Event;

class NotifyConstruct:public SCConstruct {

protected:
	Event * event;
	Value * missingEvent;
public:
	NotifyConstruct(Event * e);
	NotifyConstruct(Value * missingE);
	void setNotifiedEvent(Event * e);
	Event *getNotifiedEvent();
	std::string toString();
	static inline bool classof(const SCConstruct *scc) {
		return (scc->getID() == NOTIFYCONSTRUCT );
	}
};

#endif
