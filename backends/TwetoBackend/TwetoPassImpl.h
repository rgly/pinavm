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

#ifndef TLMBASICPASSIMPL_H
#define TLMBASICPASSIMPL_H

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
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/IntrinsicLowering.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Support/CallSite.h"
#include "llvm/Support/CFG.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/GetElementPtrTypeIterator.h"
#include "llvm/InstVisitor.h"
#include "llvm/Target/Mangler.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Config/config.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"

#include <map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <utility>

//#include "basic.h"
//#include "bus.h"

#include "Frontend.hpp"

struct SCConstruct;
struct SCCFactory;
struct SCElab;
struct Process;
struct Event;
struct SCJit;
struct Channel;
struct Port;


using namespace llvm;

namespace sc_core {
	struct sc_module;
	struct sc_process_b;
	class sc_process_host;
	class sc_time;
	class sc_simcontext;

	typedef void (sc_process_host::*SC_ENTRY_FUNC)();
	typedef void (*SC_ENTRY_FUNC_OPT)();
}
// if external/basic/basic.h is included, it links to systemc.h to
// TwetoPass.cpp. Results rtti & no-rtti conflict. So make a copy here.
namespace basic {
	typedef uint32_t addr_t;
}
extern "C" llvm::Function *
tweto_optimize_process(sc_core::SC_ENTRY_FUNC fct,
		       sc_core::sc_process_host * arg);

//============================================================================
class TwetoPassImpl {
      private:
	int callOptCounter;
	int rwCallsCounter;
	Function *writeFun;
	Function *basicWriteFun;
	Frontend *fe;
	ExecutionEngine *engine;
	bool disableMsg;
	bool is64Bit;
	SCElab *elab;
	FunctionPassManager *funPassManager;
	Module *llvmMod;

      public:
	TwetoPassImpl(Frontend * fe, ExecutionEngine * ee,
		      bool disableMsg);
	bool runOnModule(Module & M);

      private:
	void optimize(sc_core::sc_module * initiatorMod);
	void replaceCallsInProcess(sc_core::sc_module * initiatorMod,
				   sc_core::sc_process_b * proc);
	sc_core::sc_module * getTargetModule(sc_core::sc_module *
					     initiatorMod,
					     basic::addr_t a);
	Function* andOOPIsGone(Function * oldProc,
			       sc_core::sc_module * initiatorMod);
	Function* findFunction(sc_core::sc_module*, sc_core::sc_process_b*);
	void MSG(std::string msg);

};
//============================================================================


#endif