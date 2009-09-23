#ifndef _SCCONSTRUCT_HPP
#define _SCCONSTRUCT_HPP

#include <string>

#include "llvm/Instruction.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"

using namespace llvm;
using namespace std;

struct SCConstruct {
protected:
  string constructName;
  string threadName;

public:
  SCConstruct();
  string toString();
  string getThreadName();
};

#endif
