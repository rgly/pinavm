#include "SCJit.hpp"
#include "ReadHandler.hpp"
#include "Port.hpp"
#include "ReadConstruct.hpp"
#include <llvm/Type.h>

using namespace llvm;

SCConstruct *ReadHandler::handle(Function * fct, BasicBlock * bb, Instruction * callInst, Function* calledFunction)
{
	ReadConstruct* ret;

	TRACE_3("Handling call to read()\n");
	Value *arg = callInst->getOperand(1);
	void *portAddr = this->scjit->jitAddr(fct, callInst, arg);
	TRACE_4("Address jitted : " << portAddr << "\n");
	Port *p = this->scjit->getElab()->getPort(portAddr);
	TRACE_3("Port read : " << p << "\n");

	ret = new ReadConstruct(p, callInst);
	return ret;
}


void ReadHandler::insertInMap(std::map < Function *,
			      SCConstructHandler * >*scchandlers)
{
	SCConstructHandler::insertInMap(scchandlers, "_ZNK7sc_core5sc_inIbE4readEv");
	SCConstructHandler::insertInMap(scchandlers, "_ZNK7sc_core5sc_inIiE4readEv");
	SCConstructHandler::insertInMap(scchandlers, "_ZNK7sc_core5sc_inIN5sc_dt7sc_uintILi8EEEE4readEv");
}
