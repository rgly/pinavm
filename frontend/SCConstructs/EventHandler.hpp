#ifndef _EVENTHANDLER_HPP
#define _EVENTHANDLER_HPP

#include "SCConstructHandler.hpp"

using namespace llvm;

struct EventHandler:public SCConstructHandler {
      public:
	EventHandler(SCJit * jit):SCConstructHandler(jit) { }
	SCConstruct *handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction);
	void insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers);
};

#endif
