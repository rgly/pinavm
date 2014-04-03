#ifndef _METHOD_RELINKER_H
#define _METHOD_RELINKER_H

#include <systemc>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ExecutionEngine/MCJIT.h>

class MethodRelinker {
	public:
		MethodRelinker (llvm::ExecutionEngine* ee,
				sc_core::sc_simcontext* sc = 0): ee(ee), simc(sc) {
			if (!simc)
				simc = sc_core::sc_get_curr_simcontext();
		}
		virtual ~MethodRelinker() {
		}
		virtual void relinkFunction (void (**dst)(void), llvm::Function *f);
		void relinkEverything (void);
		virtual void finalize (void);
	protected:
		llvm::ExecutionEngine* ee;
		sc_core::sc_simcontext* simc;
};

class StaticMethodRelinker: public MethodRelinker {
	public:
		StaticMethodRelinker (llvm::Module* m, llvm::ExecutionEngine* ee,
				sc_core::sc_simcontext* simc = 0);
		// overrides
		virtual void relinkFunction (void (**dst)(void), llvm::Function *f);
		virtual void finalize (void);
	protected:
		llvm::Module* mod;
		llvm::Function* relinkFunc;
		llvm::IRBuilder <>* irb;
		llvm::IntegerType *intPtrType;
		llvm::LLVMContext& context;
};

#endif

