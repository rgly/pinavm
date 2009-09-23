#ifndef _ZEROTIMEHANDLER_HPP
#define _ZEROTIMEHANDLER_HPP

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "TimeHandler.hpp"
#include "ZeroTimeConstruct.hpp"

using namespace llvm;

struct ZeroTimeHandler : public TimeHandler {  
public:
  ZeroTimeHandler(SCJit* jit) : TimeHandler(jit) {}
  SCConstruct* handle(llvm::Function* fct, BasicBlock* bb, CallInst* callInst);
  void insertInMap(std::map<Function*, SCConstructHandler*>* scchandlers);
};

#endif
