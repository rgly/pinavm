#ifndef _ZEROTIMEHANDLER_HPP
#define _ZEROTIMEHANDLER_HPP

#include "TimeHandler.hpp"

using namespace llvm;

struct ZeroTimeHandler:public TimeHandler {
      public:
	ZeroTimeHandler(SCJit * jit):TimeHandler(jit) {}

	SCConstruct *handle(llvm::Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction);

	void insertInMap(std::map < Function *,
			 SCConstructHandler * >*scchandlers);
};

#endif
