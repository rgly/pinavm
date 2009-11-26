#ifndef _WRITEHANDLER_HPP
#define _WRITEHANDLER_HPP

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstructHandler.hpp"

struct Port;
struct SCJit;

using namespace llvm;

struct WriteHandler:public SCConstructHandler {
      public:
	WriteHandler(SCJit * jit):SCConstructHandler(jit) { } 
	SCConstruct *handle(Function * fct, BasicBlock * bb,
			    CallInst * callInst);
	void insertInMap(std::map < Function *,
			 SCConstructHandler * >*scchandlers);
};

#endif
