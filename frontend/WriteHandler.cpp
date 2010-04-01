#include "WriteHandler.hpp"
#include "SCJit.hpp"
#include "Port.hpp"
#include "Channel.hpp"
#include "WriteConstruct.hpp"

#include "sysc/datatypes/int/sc_uint.h"

using namespace llvm;

SCConstruct *WriteHandler::handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	WriteConstruct* ret;
	char buffer[15];

	TRACE_3("Handling call to write()\n");
	Value *arg = callInst->getOperand(1);
	void *portAddr = this->scjit->jitAddr(fct, callInst, arg);
	TRACE_4("Address jitted : " << portAddr << "\n");
	Port *po = this->scjit->getElab()->getPort(portAddr);
	TRACE_3("Port written : " << po << "\n");

 	if (po->getChannelID() == SIMPLE_CHANNEL) {
		Value *value = callInst->getOperand(2);
		Type* valueType = po->getType();
		
// 		if (this->scjit->getModule()->getTypeName(valueType) == "N5sc_dt7sc_uintILi8") {
// 			TRACE_4("handling  uint\n");
// 			sc_dt::sc_uint<8> intValue = this->scjit->jitType<sc_dt::sc_uint<8> >(fct, callInst, value);
// 			TRACE_4("got result !! \n");
// 			sprintf(buffer, "%d", intValue.to_int());
// 			ret = new WriteConstruct(po, buffer);
// 			TRACE_5("Value written is : " << buffer << "\n");
// 		} else if (valueType->isIntegerTy()) {
// 			if (((IntegerType*) valueType)->getBitWidth() == 1) {
// 				int boolValue = this->scjit->jitBool(fct, callInst, value);
// 				TRACE_5("Value written is : " << boolValue << "\n");
// 				if (boolValue) {
// 					ret = new WriteConstruct(po, "true");
// 				} else {
// 					ret = new WriteConstruct(po, "false");				
// 				}
// 			} else {
// 				int intValue = this->scjit->jitType<int>(fct, callInst, value);
// 				TRACE_5("Value written is : " << intValue << "\n");
// 				sprintf(buffer, "%d", intValue);
// 				ret = new WriteConstruct(po, buffer);
// 			}
// 		} else if(valueType->getTypeID() == Type::DoubleTyID) {
// 			double doubleValue = this->scjit->jitDouble(fct, callInst, value);
// 			TRACE_5("Value written is : " << doubleValue << "\n");
// 			sprintf(buffer, "%f", doubleValue);
// 			ret = new WriteConstruct(po, buffer);
// 		} else if (valueType->getTypeID() == Type::PointerTyID) {
			ret = new WriteConstruct(po, value);
			TRACE_5("Value written is : unknown\n");
// 		}
 	}
	return ret;
}


void WriteHandler::insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers)
{
	SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core8sc_inoutIbE5writeERKb");
	SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core8sc_inoutIiE5writeERKi");
	SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core8sc_inoutIN5sc_dt7sc_uintILi8EEEE5writeERKS3_");
}
