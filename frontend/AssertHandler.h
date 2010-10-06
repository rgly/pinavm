#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstructHandler.hpp"
#include "AssertConstruct.hpp"

using namespace llvm;

struct AssertHandler:public SCConstructHandler {
      public:
	AssertHandler(SCJit * jit):SCConstructHandler(jit) {}
	SCConstruct *handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function * calledFunction);
	void insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers);
};
