#include "SCJit.hpp"
#include "AssertHandler.h"
#include "AssertConstruct.hpp"
#include <llvm/Support/CallSite.h>

using namespace llvm;


SCConstruct *AssertHandler::handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	TRACE_3("Handling call to assert(...)\n");
	Value *arg = CallSite(callInst).getArgument(0);
	TRACE_5("callInst = "); callInst->dump();
	TRACE_5("arg = "); arg->dump();
	bool errb;
	bool cond = this->scjit->jitBool(fct, callInst, arg, &errb);
	if (errb) {
		TRACE_4("Cannot statically find argument of assert(...)\n");
		// MM: TODO: NULL should be the actual value.
		return new AssertConstruct((Value*)NULL);
	}
	TRACE_4("Cond jitted : " << cond << "\n");
	return new AssertConstruct(cond);
}

void AssertHandler::insertInMap(std::map < Function *,
				SCConstructHandler * >*scchandlers)
{
	// sc_verif::ASSERT()
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN8sc_verif6ASSERTEv");
	// sc_verif::ASSERT(bool)
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN8sc_verif6ASSERTEb");
}
