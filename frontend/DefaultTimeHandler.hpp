#ifndef _DEFAULTTIMEHANDLER_HPP
#define _DEFAULTTIMEHANDLER_HPP

#include <map>

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "TimeHandler.hpp"

struct SCJit;

using namespace llvm;

struct DefaultTimeHandler:public TimeHandler {
      public:
	DefaultTimeHandler(SCJit * jit):TimeHandler(jit) {}

	SCConstruct *handle(Function * fct, BasicBlock * bb, Instruction * callInst, Function* calledFunction);
	void insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers);
};

#endif
