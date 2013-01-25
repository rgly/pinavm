#ifndef _FUNCTIONBUILDER_H
#define _FUNCTIONBUILDER_H

#include <string>
#include <vector>

#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include "llvm/Value.h"
#include "llvm/Transforms/Utils/ValueMapper.h"

#include "config.h"

struct Process;

using namespace llvm;

class FunctionBuilder {
 private:
  std::vector<Instruction*> temp_queue;
  std::vector<Instruction*> used_insts;
  std::vector<BasicBlock*> used_bb;
  //  DenseMap<const Value*, Value*> ValueMap;
  ValueToValueMapTy ValueMap;


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
