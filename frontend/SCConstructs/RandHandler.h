#ifndef RANDHANDLER_HPP
#define RANDHANDLER_HPP


#include "SCConstructHandler.hpp"

using namespace llvm;

class RandHandler:public SCConstructHandler {
      public:
	RandHandler():SCConstructHandler() {}
	SCConstruct *handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function * calledFunction);
	void insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers);
};

#endif
