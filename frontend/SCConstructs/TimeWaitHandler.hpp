#ifndef _TIMEWAITHANDLER_HPP
#define _TIMEWAITHANDLER_HPP

#include "SCConstructHandler.hpp"

struct SCJit;

using namespace llvm;

struct TimeWaitHandler:public SCConstructHandler {
      public:
	TimeWaitHandler(SCJit * jit):SCConstructHandler(jit) {}

	SCConstruct *handle(Function * fct, BasicBlock * bb, Instruction * callInst, Function* calledFunction);
	void insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers);
};

#endif
