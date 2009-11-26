#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstructHandler.hpp"
#include "NotifyConstruct.hpp"

using namespace llvm;

struct NotifyHandler:public SCConstructHandler {
public:
  NotifyHandler(SCJit * jit):SCConstructHandler(jit) { }
  SCConstruct* handle(Function * fct, BasicBlock * bb, CallInst * callInst);
  void insertInMap(std::map < Function *, SCConstructHandler * >* scchandlers);
};
