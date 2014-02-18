#ifndef _RANDCONSTRUCT_HPP
#define _RANDCONSTRUCT_HPP

#include "SCConstruct.hpp"

using namespace llvm;

struct RandConstruct:public SCConstruct {

public:
	RandConstruct();
	std::string toString();
	static inline bool classof(const SCConstruct *scc) {
		return (scc->getID() == RANDCONSTRUCT );
	}
};

#endif
