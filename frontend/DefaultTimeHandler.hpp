#ifndef _DEFAULTTIMEHANDLER_HPP
#define _DEFAULTTIMEHANDLER_HPP

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "TimeHandler.hpp"
#include "DefaultTimeConstruct.hpp"

using namespace llvm;

struct DefaultTimeHandler : public TimeHandler {  
public:
  DefaultTimeHandler(SCJit* jit) : TimeHandler(jit) {}
  SCConstruct* handle(Function* fct, BasicBlock* bb, CallInst* callInst);
  void insertInMap(std::map<Function*, SCConstructHandler*>* scchandlers);
};

#endif
