#include "WriteHandler.hpp"
#include "SCJit.hpp"
#include "Port.hpp"
#include "Channel.hpp"
#include "WriteConstruct.hpp"

using namespace llvm;

SCConstruct *WriteHandler::handle(Function * fct, BasicBlock * bb,
				  CallInst * callInst)
{
	WriteConstruct* ret;
	char buffer[15];

	TRACE_3("Handling call to write()\n");
	Value *arg = callInst->getOperand(1);
	void *portAddr = this->scjit->jitAddr(fct, arg);
	TRACE_4("Address jitted : " << portAddr << "\n");
	Port *po = this->scjit->getElab()->getPort(portAddr);
	TRACE_3("Port written : " << po << "\n");

	if (po->getChannelID() == SIMPLE_CHANNEL) {
		Value *value = callInst->getOperand(2);
		Type* valueType = po->getType();

		if (valueType->isIntegerTy()) {
			if (((IntegerType*) valueType)->getBitWidth() == 1) {
				char* valuePointer= (char*) this->scjit->jitAddr(fct, value);
				int boolValue = (int) *valuePointer;
				if (boolValue) {
					ret = new WriteConstruct(po, "true");
				} else {
					ret = new WriteConstruct(po, "false");				
				}
			} else {
				int intValue = this->scjit->jitInt(fct, value);
				sprintf(buffer, "%d", intValue);
				ret = new WriteConstruct(po, buffer);
			}
		} else if(valueType->getTypeID() == Type::DoubleTyID) {
			double doubleValue = this->scjit->jitDouble(fct, value);
			sprintf(buffer, "%f", doubleValue);
			ret = new WriteConstruct(po, buffer);
		} else if (valueType->getTypeID() == Type::PointerTyID) {
			ret = new WriteConstruct(po, value);
		}
	}

	return ret;
}


void WriteHandler::insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers)
{
	SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core8sc_inoutIbE5writeERKb");
}
