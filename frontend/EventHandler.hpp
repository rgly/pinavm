#ifndef _EVENTHANDLER_HPP
#define _EVENTHANDLER_HPP

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstructHandler.hpp"
#include "EventConstruct.hpp"
#include "Event.hpp"

using namespace llvm;

struct EventHandler:public SCConstructHandler {
      public:
	EventHandler(SCJit * jit):SCConstructHandler(jit) {
	} SCConstruct *handle(Function * fct, BasicBlock * bb,
			      CallInst * callInst);
	void insertInMap(std::map < Function *,
			 SCConstructHandler * >*scchandlers);
};

#endif
