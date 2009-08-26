#ifndef _ZEROTIMECONSTRUCT_HPP
#define _ZEROTIMECONSTRUCT_HPP

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "TimeConstruct.hpp"

using namespace llvm;

struct ZeroTimeConstruct : public TimeConstruct {  
public:
  ZeroTimeConstruct();
  char* toString();
};

#endif
