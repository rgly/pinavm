#include "SCJit.hpp"
#include "DefaultTimeHandler.hpp"
#include "DefaultTimeConstruct.hpp"
#include <llvm/Support/CallSite.h>
#include "sysc/kernel/sc_time.h"


using namespace llvm;

static time_unit sc2pinavm_time_unit(sc_core::sc_time_unit sc_tu) {
	switch (sc_tu) {
	case SC_FS : return SC_FS;
	case SC_PS : return SC_PS;
	case SC_NS : return SC_NS;
	case SC_US : return SC_US;
	case SC_MS : return SC_MS;
	case SC_SEC : return SC_SEC;
	default : return SC_NS;
	}
}


SCConstruct *DefaultTimeHandler::handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	TRACE_3("Handling call to wait(...)");
	// arg0 is the module, argument 1 is the time to wait.
	Value *arg = CallSite(callInst).getArgument(1);
	bool errb = false;

	Value* arg2 = NULL;
	Function* callee = CallSite(callInst).getCalledFunction();
	unsigned arg_num = callee->getArgumentList().size();
	if (arg_num > 2)
		arg2 = CallSite(callInst).getArgument(2);


	if (arg->getType()->isIntegerTy()) {
		int time_waited = this->scjit->jitInt(fct, callInst, arg, &errb);
		// MM: TODO: check errb.
		// MM: TODO: this is _not_ a wait on time semantically.
		TRACE_3("Int time waited: " << time_waited << "\n");
		return new DefaultTimeConstruct(time_waited);
	} else if (arg->getType()->isFloatingPointTy()) {
		double time_waited = this->scjit->jitDouble(fct, callInst, arg, &errb);
		// MM: TODO: check errb.
		TRACE_3("Double time waited: " << time_waited << "\n");

		if (arg2) {
			sc_core::sc_time_unit time_u =
				this->scjit->jitType<sc_core::sc_time_unit>(fct, callInst, arg2, &errb);

			return new DefaultTimeConstruct(time_waited,
							sc2pinavm_time_unit(time_u));
		} else {
			return new DefaultTimeConstruct(time_waited);
		}
	} else {
		ERROR("call to wait with argument not int nor double\n");
	}
}

void DefaultTimeHandler::insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers)
{
	// sc_core::sc_module::wait(int)
	// i.e. wait for <int> delta-cycles.
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core9sc_module4waitEi");
	// sc_core::sc_module::wait(double, sc_core::sc_time_unit)
	// i.e. wait for some time.
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core9sc_module4waitEdNS_12sc_time_unitE");
	// sc_core::wait(double, sc_core::sc_time_unit, sc_core::sc_simcontext*)
	// wait(double, sc_time_unit) seems to be replaced by this
	// sometimes.
	// MM: TODO: this is sc_core::wait, hence not a member
	// function. Relevant argument is probably arg 0 instead of arg 1 here.
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core4waitEdNS_12sc_time_unitEPNS_13sc_simcontextE");
}
