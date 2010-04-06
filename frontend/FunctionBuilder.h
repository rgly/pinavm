#ifndef _FUNCTIONBUILDER_H
#define _FUNCTIONBUILDER_H

#include <string>
#include <iterator>
#include <queue>
#include <iostream>
#include <algorithm>
#include <vector>

#include "llvm/Transforms/Utils/ValueMapper.h"
#include "llvm/Transforms/Utils/Cloning.h"


#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/Transforms/Scalar.h"

#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetSelect.h"

#include "llvm/Analysis/Dominators.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/PassManager.h"
#include "llvm/Value.h"
#include "llvm/User.h"
#include "llvm/Module.h"
#include "llvm/Type.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include "llvm/Support/IRBuilder.h"
#include "llvm/CallingConv.h"
#include "llvm/TypeSymbolTable.h"

#include "Process.hpp"
#include "FUtils.hpp"
#include "config.h"

using namespace std;
using namespace llvm;

class FunctionBuilder {
 private:
  vector<Instruction*> temp_queue;
  vector<Instruction*> used_insts;
  vector<BasicBlock*> used_bb;
  DenseMap<const Value*, Value*> ValueMap;

  Process* proc;
  Function* origFct;
  Function* fctToJit;
  Value* res;
  Instruction* targetInst;

  bool mark(Value* arg);
  int markUsefulInstructions();
  void cloneBlocks ();
  std::vector<Instruction*>* predecessors;

 public:
  FunctionBuilder(Function* origFunction, Function* functionToJit, Instruction* inst, Value* resValue);
  ~FunctionBuilder();
  Function* buildFct();
  bool isBeforeTargetInst(Value* v);
};

#endif
