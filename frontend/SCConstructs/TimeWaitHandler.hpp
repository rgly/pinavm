#ifndef _TIMEWAITHANDLER_HPP
#define _TIMEWAITHANDLER_HPP

#include "SCConstructHandler.hpp"

class SCJit;

using namespace llvm;

class TimeWaitHandler:public SCConstructHandler {
      public:
	TimeWaitHandler(SCJit * jit):SCConstructHandler(jit) {}

	SCConstruct *handle(Function * fct, BasicBlock * bb, Instruction * callInst, Function* calledFunction);
	void insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers);
};

#endif
