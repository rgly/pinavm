#ifndef _WRITEHANDLER_HPP
#define _WRITEHANDLER_HPP


#include "SCConstructHandler.hpp"

struct SCJit;

using namespace llvm;

struct WriteHandler:public SCConstructHandler {
      public:
	WriteHandler(SCJit * jit):SCConstructHandler(jit) {}

	SCConstruct *handle(Function * fct, BasicBlock * bb, Instruction * callInst, Function* calledFunction);
	void insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers);
};

#endif
