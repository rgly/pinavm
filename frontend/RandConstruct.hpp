#ifndef _RANDCONSTRUCT_HPP
#define _RANDCONSTRUCT_HPP

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstruct.hpp"

using namespace llvm;

struct RandConstruct:public SCConstruct {

public:
	RandConstruct();
	string toString();
};

#endif
