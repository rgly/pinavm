#ifndef _RANDCONSTRUCT_HPP
#define _RANDCONSTRUCT_HPP

#include "SCConstruct.hpp"

using namespace llvm;

struct RandConstruct:public SCConstruct {

public:
	RandConstruct();
	std::string toString();
};

#endif
