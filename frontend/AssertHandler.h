#ifndef ASSERTHANDLER_HPP
#define ASSERTHANDLER_HPP

#include "SCConstructHandler.hpp"

using namespace llvm;

struct AssertHandler:public SCConstructHandler {
      public:
	AssertHandler(SCJit * jit):SCConstructHandler(jit) {}
	SCConstruct *handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function * calledFunction);
	void insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers);
};

#endif
