#ifndef _SCCONSTRUCT_HPP
#define _SCCONSTRUCT_HPP

#include "llvm/Instruction.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"

using namespace llvm;

struct SCConstruct {
protected:
  char* constructName;
  char* threadName;

public:
  SCConstruct();
  char* toString();
  char* getThreadName();
};

#endif
