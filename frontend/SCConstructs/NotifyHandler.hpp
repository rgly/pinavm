#ifndef NOTIFYHANDLER_HPP
#define NOTIFYHANDLER_HPP

#include "SCConstructHandler.hpp"

using namespace llvm;

struct NotifyHandler:public SCConstructHandler {
      public:
	NotifyHandler(SCJit * jit):SCConstructHandler(jit) {}
	SCConstruct *handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function * calledFunction);
	void insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers);
};

#endif
