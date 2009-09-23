#ifndef _SCJIT_HPP
#define _SCJIT_HPP

#include <string>
#include <iterator>
#include <queue>
#include <iostream>
#include <algorithm>


#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Analysis/Verifier.h"

#include "llvm/Transforms/Utils/ValueMapper.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/Transforms/Scalar.h"

#include "llvm/Target/TargetData.h"

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
#include <llvm/ModuleProvider.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include "llvm/Support/IRBuilder.h"
#include <llvm/ExecutionEngine/GenericValue.h>
#include "llvm/CallingConv.h"

using namespace std;
using namespace llvm;

struct SCJit {
private:
  Module* mdl;
  ExecutionEngine* ee;
  void buildFctToJit(Function* f, Function* fctToJit, Value* arg);
  
public:
  SCJit(Module* mod);
  void doFinalization();
  void elaborate();  
  int jitInt(Function* f, Value* arg);
  Module* getModule();
};

#endif
