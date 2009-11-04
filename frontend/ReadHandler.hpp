#ifndef _READHANDLER_HPP
#define _READHANDLER_HPP

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstructHandler.hpp"
#include "ReadConstruct.hpp"
#include "SCPort.hpp"

using namespace llvm;

struct ReadHandler:public SCConstructHandler {
      public:
	ReadHandler(SCJit * jit):SCConstructHandler(jit) {
	};
	SCConstruct *handle(Function * fct, BasicBlock * bb,
			    CallInst * callInst);
	void insertInMap(std::map < Function *,
			 SCConstructHandler * >*scchandlers);
};

#endif
