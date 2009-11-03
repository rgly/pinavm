#ifndef SCCFACTORY_HPP
#define SCCFACTORY_HPP

#include <map>

#include "llvm/Function.h"
#include "llvm/BasicBlock.h"
#include "llvm/Instruction.h"

#include "SCJit.hpp"

#include "SCConstructHandler.hpp"
#include "EventHandler.hpp"
#include "DefaultTimeHandler.hpp"
#include "NotifyHandler.hpp"
#include "DefaultTimeHandler.hpp"
#include "ZeroTimeHandler.hpp"
#include "WriteHandler.hpp"
#include "ReadHandler.hpp"

#include "SCConstruct.hpp"
#include "EventConstruct.hpp"
#include "DefaultTimeConstruct.hpp"
#include "NotifyConstruct.hpp"
#include "DefaultTimeConstruct.hpp"
#include "ZeroTimeConstruct.hpp"
#include "WriteConstruct.hpp"
#include "ReadConstruct.hpp"


struct SCCFactory {

private:
  std::map<Function*, SCConstructHandler*> scchandlers;
  std::map<CallInst*, SCConstruct*> scc;
public:
  SCCFactory(SCJit* jit);
  bool handle(llvm::Function* fct, BasicBlock* bb, CallInst* callInst);
  
};

#endif
