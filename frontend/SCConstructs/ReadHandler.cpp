#include "SCJit.hpp"
#include "ReadHandler.hpp"
#include "ReadConstruct.hpp"

#include <llvm/Type.h>
#include <llvm/Support/CallSite.h>

using namespace llvm;

SCConstruct *ReadHandler::handle(Function * fct, BasicBlock * bb, Instruction * callInst, Function* calledFunction)
{
	ReadConstruct* ret;

	TRACE_3("Handling call to read()\n");
	// Analyzing "arg.read()"
	Value *arg = CallSite(callInst).getArgument(0);
	void *portAddr = this->scjit->jitAddr(fct, callInst, arg);
	if (portAddr == NULL)
		return new ReadConstruct(NULL, callInst);

	TRACE_4("Address jitted : " << portAddr << "\n");
	Port *p = this->scjit->getElab()->getPort(portAddr);
	TRACE_3("Port read : " << p << "\n");

	ret = new ReadConstruct(p, callInst);
	return ret;
}


void ReadHandler::insertInMap(std::map < Function *,
			      SCConstructHandler * >*scchandlers)
{
	// sc_core::sc_in<bool>::read() const
	SCConstructHandler::insertInMap(scchandlers, "_ZNK7sc_core5sc_inIbE4readEv");
	// sc_core::sc_in<int>::read() const
	SCConstructHandler::insertInMap(scchandlers, "_ZNK7sc_core5sc_inIiE4readEv");
	// sc_core::sc_in<sc_dt::sc_uint<8> >::read() const
	SCConstructHandler::insertInMap(scchandlers, "_ZNK7sc_core5sc_inIN5sc_dt7sc_uintILi8EEEE4readEv");
}
