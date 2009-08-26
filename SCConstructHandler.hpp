#ifndef _SCCONSTRUCTHANDLER_HPP
#define _SCCONSTRUCTHANDLER_HPP

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Streams.h"

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstruct.hpp"
#include "SCJit.hpp"

using namespace llvm;

class SCConstructHandler {
protected:
  SCJit* scjit;

public:
  SCConstructHandler(SCJit* jit);
  SCConstructHandler();
  virtual SCConstruct* handle(llvm::Function* fct, BasicBlock* bb, CallInst* callInst) = 0;
  void insertInMap(std::map<Function*, SCConstructHandler*>* scchandlers, char* fctName);
};

#endif
