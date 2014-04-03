#ifndef _FUNCTIONBUILDER_H
#define _FUNCTIONBUILDER_H

#include <string>
#include <vector>

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/Transforms/Utils/ValueMapper.h"

#include "config.h"

class Process;

using namespace llvm;

class FunctionBuilder {
 private:
  std::vector<Instruction*> temp_queue;
  std::vector<Instruction*> used_insts;
  std::vector<BasicBlock*> used_bb;
  //  DenseMap<const Value*, Value*> ValueMap;
  ValueToValueMapTy ValueMap;


  Function* origFct;
  Function* fctToJit;
  Value* res;
  Instruction* targetInst;

  bool mark(Value* arg);
  bool markUsefulInstructions();
  void cloneBlocks ();
  std::vector<Instruction*>* predecessors;

 public:
  FunctionBuilder(Function* origFunction, Function* functionToJit, Instruction* inst, Value* resValue);
  ~FunctionBuilder();
  Function* buildFct();
  bool isBeforeTargetInst(Value* v);

 private:
  std::vector<Instruction*>* fillPredecessors(Function* origFct,
		Instruction* targetInst);
  void cloneEachInst(BasicBlock* origbb, BasicBlock* NewBB);
  void cloneArguments(Function* origFct, Function* fctToJit);
  BasicBlock* fillClonedBBWithMarkedInst();
  void remapOperands();
  void CreateReturn(BasicBlock* lastBlock);

  bool isMarked(Instruction*);
  bool isMarked(BasicBlock*);
};

#endif
