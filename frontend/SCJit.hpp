#ifndef _SCJIT_HPP
#define _SCJIT_HPP

#include <string>
#include <iterator>
#include <queue>
#include <iostream>
#include <algorithm>

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"

//#include "llvm/Analysis/Verifier.h"

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
#include <llvm/ModuleProvider.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include "llvm/Support/IRBuilder.h"
#include "llvm/CallingConv.h"
#include "llvm/TypeSymbolTable.h"

#include "llvm/ExecutionEngine/JIT.h"
#include <llvm/ExecutionEngine/GenericValue.h>

#include "SCElab.h"
#include "FunctionBuilder.h"
#include "Process.hpp"
#include "config.h"

using namespace std;
using namespace llvm;

struct SCJit {
      private:
	Module * mdl;
	SCElab *elab;
	Process *currentProcess;
	ExecutionEngine *ee;
	ExistingModuleProvider *moduleProvider;

      public:
	 SCJit(Module * mod, SCElab * scelab);
	~SCJit();

	void doFinalization();
	void elaborate();
	SCElab *getElab();
	void setCurrentProcess(Process * process);
	Process *getCurrentProcess();
	void *jitAddr(Function * f, Value * arg);
	int jitInt(Function * f, Value * arg);
	double jitDouble(Function * f, Value * arg);
	bool jitBool(Function * f, Value * arg);
	Function *buildFct(Function * f, FunctionType * FT, Value * arg);
	Module *getModule();
};

#endif
