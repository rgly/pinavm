#ifndef _SCJIT_HPP
#define _SCJIT_HPP

#include <string>
#include <iterator>
#include <queue>
#include <iostream>
#include <algorithm>

#include "sysc/datatypes/int/sc_uint.h"


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
	int nbFctToJit;

      public:
	 SCJit(Module * mod, SCElab * scelab);
	~SCJit();

	void doFinalization();
	void elaborate();
	SCElab *getElab();
	void setCurrentProcess(Process * process);
	Process *getCurrentProcess();
	void fillArgsType(Function * f, std::vector < const Type * >*argsType);
	void *jitAddr(Function * f, Instruction* inst, Value * arg);
	int jitInt(Function * f, Instruction* inst, Value * arg, bool* errb);
	double jitDouble(Function * f, Instruction* inst, Value * arg, bool* errb);
	bool jitBool(Function * f, Instruction* inst, Value * arg, bool* errb);
	Function *buildFct(Function * f, FunctionType * FT, Instruction* inst, Value * arg);
	Module *getModule();

	template<class RetTy>
	RetTy jitType(Function * f, Instruction* inst, Value * arg, bool* errb) {
		Function *fctToJit;
		const std::vector < const Type *>argsType;
		
		TRACE_5("jitType() \n");
		
		fillArgsType(f, (std::vector < const Type * >*) &argsType);
		FunctionType *FT;
		if (isa<PointerType>(arg->getType())) {
			const Type* pt = dyn_cast<PointerType>(arg->getType())->getElementType();
			FT = FunctionType::get(pt, argsType, false);
		} else
			FT = FunctionType::get(arg->getType(), argsType, false);
		
		fctToJit = buildFct(f, FT, inst, arg);
		if (fctToJit == NULL) {
			*errb = true;
			return (RetTy) 0;
		}

		RetTy (*fct) (sc_core::sc_module *) =
			(RetTy (*)(sc_core::sc_module *)) ee->getPointerToFunction(fctToJit);
		
		IRModule* mod = this->getCurrentProcess()->getModule();
		TRACE_4("********************* SC MODULE : " << mod << "\n");
		RetTy res = fct(this->elab->getSCModule(mod));
		
		fctToJit->dropAllReferences();
		ee->freeMachineCodeForFunction(fctToJit);
		fctToJit->eraseFromParent();
		
		return res;
	}
	
};

#endif
