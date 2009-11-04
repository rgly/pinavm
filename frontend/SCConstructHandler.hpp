#ifndef _SCCONSTRUCTHANDLER_HPP
#define _SCCONSTRUCTHANDLER_HPP

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstruct.hpp"
#include "SCJit.hpp"

using namespace llvm;

class SCConstructHandler {
      protected:
	SCJit * scjit;

      public:
	SCConstructHandler(SCJit * jit);
	SCConstructHandler();
	virtual SCConstruct *handle(llvm::Function * fct, BasicBlock * bb,
				    CallInst * callInst) = 0;
	void insertInMap(std::map < Function *,
			 SCConstructHandler * >*scchandlers,
			 string fctName);
};

#endif
