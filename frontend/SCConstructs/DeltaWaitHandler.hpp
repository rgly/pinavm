#ifndef _DELTAWAITHANDLER_HPP
#define _DELTAWAITHANDLER_HPP

#include "SCConstructHandler.hpp"

using namespace llvm;

class DeltaWaitHandler:public SCConstructHandler {
      public:
	DeltaWaitHandler(SCJit * jit):SCConstructHandler(jit) {}

	SCConstruct *handle(llvm::Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction);

	void insertInMap(std::map < Function *,
			 SCConstructHandler * >*scchandlers);
};

#endif
