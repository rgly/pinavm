#ifndef _EVENTWAITHANDLER_HPP
#define _EVENTWAITHANDLER_HPP

#include "SCConstructHandler.hpp"

using namespace llvm;

struct EventWaitHandler:public SCConstructHandler {
      public:
	EventWaitHandler(SCJit * jit):SCConstructHandler(jit) { }
	SCConstruct *handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction);
	void insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers);
};

#endif
