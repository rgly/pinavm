#ifndef _EVENTWAITCONSTRUCT_HPP
#define _EVENTWAITCONSTRUCT_HPP

#include "WaitConstruct.hpp"
#include "Event.hpp"

class EventWaitConstruct:public WaitConstruct {
private:
	Event * event;
	llvm::Value * missingEvent;
public:
	EventWaitConstruct(llvm::Value* missingE);
	EventWaitConstruct(Event * e);
//	void setWaitedEvent(Event * e);
	llvm::Value* getMissingEvent();
	Event *getEvent() const;
	std::string toString();

	static inline bool classof(const SCConstruct *scc) {
		if (llvm::isa<WaitConstruct>(scc)) {
			const WaitConstruct* wc = llvm::dyn_cast<WaitConstruct>(scc);
			return EventWaitConstruct::classof(wc);
		} else
			return false;
	}

	static inline bool classof(const WaitConstruct *wc) {
		return (wc->getWaitID() == EVENT);
	}
};

#endif
