#ifndef _DEFAULTTIMEHANDLER_HPP
#define _DEFAULTTIMEHANDLER_HPP

#include <map>

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
