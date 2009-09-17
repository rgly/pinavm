#ifndef _DEFAULTTIMECONSTRUCT_HPP
#define _DEFAULTTIMECONSTRUCT_HPP

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "TimeConstruct.hpp"

using namespace llvm;

struct DefaultTimeConstruct : public TimeConstruct {
  DefaultTimeConstruct(int t) : TimeConstruct::TimeConstruct(t) {}
  char* toString();
};

#endif
