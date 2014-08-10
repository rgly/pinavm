#include "MethodRelinker.h"
#include <iostream>
#include <assert.h>
#include "sysc/kernel/sc_process_table.h"
#include "sysc/kernel/sc_process.h"
#include "sysc/pinavm/permalloc.h"
#include <llvm/IR/Verifier.h>

using namespace llvm;
using namespace sc_core;

extern GlobalVariable* sbase;

StaticMethodRelinker::StaticMethodRelinker (Module* m, ExecutionEngine* ee,
                                            sc_core::sc_simcontext* simc)
                                           :MethodRelinker (ee, simc), mod (m),
					    context (getGlobalContext())
{
	if (sizeof(void*)==8) {
		intPtrType = Type::getInt64Ty(context);
	} else {
		intPtrType = Type::getInt32Ty(context);
	}
	FunctionType* funType = FunctionType::get (Type::getVoidTy(context), false);
	relinkFunc = Function::Create(funType, Function::ExternalLinkage,
	                              "relink_processes", mod);
	BasicBlock* bb = BasicBlock::Create (context, "entry", relinkFunc);
	irb = new IRBuilder<> (context);
	irb->SetInsertPoint (bb);
}

void MethodRelinker::relinkFunction (void (**dst)(void), llvm::Function* f)
{
	if (!f) return;
	void* p = ee->getPointerToFunction (f);
	*dst = reinterpret_cast<void(*)(void)>(p);
}

void StaticMethodRelinker::relinkFunction (void (**dst)(void), llvm::Function* f)
{
	if (!f) return;
	MethodRelinker::relinkFunction (dst, f);
	if (permalloc::is_from (dst)) {
		ptrdiff_t off = permalloc::get_offset(dst);
		std::cout << "perm store fun " << f->getName().str() << " at addr "
			  << dst << " off " << std::hex << off << std::endl;
		// now, do the relocation in the relink_processes function
		ConstantInt *offset = ConstantInt::getSigned(intPtrType, off);
		Value* sbase_val = irb->CreateLoad (sbase);
		Value* pi8Addr = irb->CreateGEP
			(sbase_val, std::vector<Value*>(1,offset));
		Type* type = f->getFunctionType();
		Value* addr = irb->CreateBitCast (pi8Addr, type->getPointerTo()
				->getPointerTo());
		irb->CreateStore (f, addr);
	} else {
		assert (false && "couldn't relink !");
	}
}

void MethodRelinker::finalize (void)
{
}

void StaticMethodRelinker::finalize (void)
{
	irb->CreateRetVoid ();
	verifyFunction (*relinkFunc);
	relinkFunc->dump();
}

void MethodRelinker::relinkEverything (void)
{
	sc_core::sc_process_table * processes = 
		simc->m_process_table;

	// for each SC_THREAD
	sc_core::sc_thread_handle thread_p;
	for (thread_p = processes->thread_q_head(); 
	     thread_p; thread_p = thread_p->next_exist()) {
		sc_core::sc_process_b *proc = thread_p;
		Function *procf = proc->m_bc_semantics_p;
		relinkFunction (&proc->m_semantics_p, procf);
	}

	// for each SC_METHOD
	sc_core::sc_method_handle method_p;
	for (method_p = processes->method_q_head(); 
	     method_p; method_p = method_p->next_exist()) {
		sc_core::sc_process_b *proc = method_p;
		Function *procf = proc->m_bc_semantics_p;
		relinkFunction (&proc->m_semantics_p, procf);
	}

	finalize ();
}

