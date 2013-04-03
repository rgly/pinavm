#ifndef _SCCONSTRUCTHANDLER_HPP
#define _SCCONSTRUCTHANDLER_HPP

#include <map>
#include <string>

#include "llvm/Function.h"

#include "config.h"

using namespace llvm;

struct SCJit;
struct SCConstruct;

class SCConstructHandler {
      protected:
	SCJit * scjit;

      public:
	SCConstructHandler(SCJit * jit);
	SCConstructHandler();
	virtual ~SCConstructHandler(){};
	virtual SCConstruct *handle(Function * fct, BasicBlock * bb, Instruction * callInst, Function* calledFunction) = 0;
	void insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers, std::string fctName);
};

#endif
