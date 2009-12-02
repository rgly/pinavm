#ifndef _SCCFACTORY_HPP
#define _SCCFACTORY_HPP

#include <map>

#include "llvm/Function.h"
#include "llvm/BasicBlock.h"
#include "llvm/Instruction.h"

#include "SCConstructHandler.hpp"
#include "EventHandler.hpp"
#include "DefaultTimeHandler.hpp"
#include "NotifyHandler.hpp"
#include "ZeroTimeHandler.hpp"
#include "WriteHandler.hpp"
#include "ReadHandler.hpp"

struct SCCFactory {

      private:
	std::map < Function *, SCConstructHandler * >scchandlers;
	std::map < CallInst *, SCConstruct * >scc;

      public:
	SCCFactory(SCJit * jit);
	~SCCFactory();
	bool handle(llvm::Function * fct, BasicBlock * bb,
		    CallInst * callInst);
	 std::map < CallInst *, SCConstruct * >*getConstructs();
};

#endif
