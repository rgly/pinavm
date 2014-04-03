#ifndef _TIMEWAITCONSTRUCT_HPP
#define _TIMEWAITCONSTRUCT_HPP

#include "llvm/IR/Value.h"

#include "WaitConstruct.hpp"
#include "Time.hpp"

class TimeWaitConstruct:public WaitConstruct{
private:
	pinavm::Time time;
	llvm::Value* missingTime;
public:
	TimeWaitConstruct(llvm::Value* missingT);
	TimeWaitConstruct(pinavm::Time t);
	pinavm::Time getTime() const;
	llvm::Value* getMissingTime();
	std::string toString();

	static inline bool classof(const SCConstruct *scc) {
		if (llvm::isa<WaitConstruct>(scc)) {
			const WaitConstruct* wc = llvm::dyn_cast<WaitConstruct>(scc);
			return TimeWaitConstruct::classof(wc);
		} else
			return false;
	}

	static inline bool classof(const WaitConstruct *wc) {
		return (wc->getWaitID() == TIMED);
	}
};

#endif
