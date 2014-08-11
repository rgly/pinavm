/**
 * TwetoPassImpl.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : si-mohamed.lamraoui@imag.fr
 * @date : 2011/03/17
 * @copyright : Verimag 2011
 */

#ifndef TWETOPASSIMPL_H
#define TWETOPASSIMPL_H

#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Analysis/ConstantsScanner.h"
#include "llvm/Analysis/FindUsedTypes.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/IntrinsicLowering.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/CFG.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/IR/GetElementPtrTypeIterator.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/Target/Mangler.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Config/config.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/IRBuilder.h"

#include <map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <utility>

#include "Frontend.hpp"
#include "TwetoBackend.h"

class SCConstruct;
class SCCFactory;
class SCElab;
class Process;
class Event;
class SCJit;
class Channel;
class Port;
class TwetoPass;


using namespace llvm;

namespace sc_core {
	class sc_module;
	class sc_process_b;
	class sc_process_host;
	class sc_time;
	class sc_simcontext;
}

// if external/basic/basic.h is included, it links to systemc.h to
// TwetoPass.cpp. Results rtti & no-rtti conflict. So make a copy here.
namespace basic {
	typedef uint32_t addr_t;
	class target_module_base;
}

// Handy structure that keeps the 
// informations for a possible call creation
struct CallInfo {
	// Target module info 
	// the value is an integer, but ConstantInt would require upcasts
	basic::target_module_base *targetMod;
	const llvm::Type *targetType;
	// Argument info
	llvm::Value *dataArg;
	llvm::Value *addrArg;
	// Call info
	llvm::Instruction *oldcall;
	llvm::Instruction *newcall;
	// Nature of the memory op
	basic::addr_t offset;
	bool isWrite;
};

//============================================================================
class TwetoPassImpl {
	friend class TwetoPass;
      private:
	int callOptCounter;
	int rwCallsCounter;
	Function *writeFun;
	Function *basicWriteFun;
	Frontend *fe;
	ExecutionEngine *engine;
	enum tweto_opt_level optlevel;
	bool disableMsg;
	SCElab *elab;
	FunctionPassManager *funPassManager;
	ScalarEvolution* se;
	Module *llvmMod;
	Type *intptrType;
	TwetoPass* parent;

      public:
	TwetoPassImpl(Frontend * fe, ExecutionEngine * ee,
		      enum tweto_opt_level optimize, bool disableMsg);
	bool runOnModule(Module & M);
	void getAnalysisUsage (llvm::AnalysisUsage& AU);

      private:
	void optimize(sc_core::sc_module * initiatorMod);
	void inlineBasicIO(sc_core::sc_module*, llvm::Function*);
	Function* andOOPIsGone(Function * oldProc,
			       sc_core::sc_module * initiatorMod);
	Function* findFunction(sc_core::sc_module*, sc_core::sc_process_b*);
	Value* createRelocatablePointer (Type*, void*, IRBuilder<>*);
	void MSG(std::string msg);

};
//============================================================================


#endif
