#ifndef _SCJIT_HPP
#define _SCJIT_HPP

#include "SCElab.h"
#include "FunctionBuilder.h"
#include "Process.hpp"
#include "config.h"

#include <llvm/ExecutionEngine/ExecutionEngine.h>

#ifdef __GNUC__
#define DEPRECATED __attribute__((deprecated))
#else
#define DEPRECATED
#endif

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
	void fillArgsType(Function * f, std::vector <Type * >*argsType);
	void *jitAddr(Function * f, Instruction* inst, Value * arg)
		DEPRECATED;
	int jitInt(Function * f, Instruction* inst, Value * arg, bool* errb)
		DEPRECATED;
	double jitDouble(Function * f, Instruction* inst, Value * arg, bool* errb)
		DEPRECATED;
	bool jitBool(Function * f, Instruction* inst, Value * arg, bool* errb)
		DEPRECATED;
	Function *buildFct(Function * f, FunctionType * FT, Instruction* inst, Value * arg);
	Module *getModule();
	ExecutionEngine* getEngine();

	template<class RetTy>
	RetTy jitType(Function * f, Instruction* inst, Value * arg, bool* errb) {
		Function *fctToJit;
		const std::vector <Type *>argsType;
		
		TRACE_5("jitType() \n");
		
		fillArgsType(f, (std::vector <Type * >*) &argsType);

		Type* ret_arg_ty = arg->getType();
		// don't automatically dereference pointers since it could
		// be specialized to void*
		FunctionType *FT = FunctionType::get(ret_arg_ty, ArrayRef<Type *>(argsType), false);
		
		fctToJit = buildFct(f, FT, inst, arg);
		if (fctToJit == NULL) {
			if (errb)
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
