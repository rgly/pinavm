#ifndef _DELTAWAITCONSTRUCT_HPP
#define _DELTAWAITCONSTRUCT_HPP

#include "WaitConstruct.hpp"
#include "llvm/Value.h"


struct DeltaWaitConstruct:public WaitConstruct {
private:
	llvm::Value* missingDelta;
	int Delta;
public:
	DeltaWaitConstruct(int);
	DeltaWaitConstruct(llvm::Value* missingT);
	int getDelta() const;
	llvm::Value* getMissingDelta();
	std::string toString();

	static inline bool classof(const SCConstruct *scc) {
		if (llvm::isa<WaitConstruct>(scc)) {
			const WaitConstruct* wc = llvm::dyn_cast<WaitConstruct>(scc);
			return DeltaWaitConstruct::classof(wc);
		} else
			return false;
	}

	static inline bool classof(const WaitConstruct *wc) {
		return (wc->getWaitID() == DELTA);
	}
};

#endif
