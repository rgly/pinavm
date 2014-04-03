#ifndef _READHANDLER_HPP
#define _READHANDLER_HPP


#include "SCConstructHandler.hpp"

using namespace llvm;

class SCJit;

class ReadHandler:public SCConstructHandler {
      public:
	ReadHandler(SCJit * jit):SCConstructHandler(jit) {
	};
	SCConstruct *handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction);
	void insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers);
};

#endif
