#ifndef _SCJIT_HPP
#define _SCJIT_HPP

#include "SCElab.h"
#include "FunctionBuilder.h"
#include "Process.hpp"
#include "config.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>


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
	ExecutionEngine* getEngine();

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
