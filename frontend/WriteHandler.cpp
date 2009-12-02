#include "WriteHandler.hpp"
#include "SCJit.hpp"
#include "Port.hpp"
#include "WriteConstruct.hpp"

using namespace llvm;

SCConstruct *WriteHandler::handle(Function * fct, BasicBlock * bb,
				  CallInst * callInst)
{
	TRACE_3("Handling call to write()\n");

	Value *arg = callInst->getOperand(2);
	void *portAddr = this->scjit->jitAddr(fct, arg);
	TRACE_4("Address jitted : " << portAddr << "\n");
	Port *p = this->scjit->getElab()->getPort(portAddr);
	TRACE_3("Port read : " << p << "\n");
	return new WriteConstruct(p);
}


void WriteHandler::insertInMap(std::map < Function *,
			       SCConstructHandler * >*scchandlers)
{
	SCConstructHandler::insertInMap(scchandlers,
					"_ZN7sc_core8sc_inoutIbE5writeERKb");
}
