#ifndef _ASSERTCONSTRUCT_HPP
#define _ASSERTCONSTRUCT_HPP

#include "llvm/IR/Value.h"

#include "SCConstruct.hpp"
#include "Event.hpp"

using namespace llvm;

struct Event;

struct AssertConstruct:public SCConstruct {

protected:
	bool cond;
	Value * missingCond;
public:
	AssertConstruct(bool cond);
	AssertConstruct(Value * missingE);
	void setCond(bool b);
	bool getCond();
	Value* getMissingCond();
	std::string toString();
	static inline bool classof(const SCConstruct *scc) {
		return (scc->getID() == ASSERTCONSTRUCT );
	}
};

#endif
