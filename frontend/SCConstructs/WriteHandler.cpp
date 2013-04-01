#include "WriteHandler.hpp"
#include "SCJit.hpp"
#include "Port.hpp"
#include "Channel.hpp"
#include "WriteConstruct.hpp"

#include "sysc/datatypes/int/sc_uint.h"
#include <llvm/Support/CallSite.h>

using namespace llvm;

SCConstruct *WriteHandler::handle(Function * fct, BasicBlock * bb, Instruction* callInst, Function* calledFunction)
{
	WriteConstruct* ret;
	char buffer[15];

	TRACE_3("Handling call to write()\n");
	// Analyzing "arg.write(value)"
	Value *arg = CallSite(callInst).getArgument(0);
	Value *value = CallSite(callInst).getArgument(1);
	Port *po = NULL;

	void *portAddr = this->scjit->jitAddr(fct, callInst, arg);
	TRACE_4("Address jitted : " << portAddr << "\n");
	if (portAddr == NULL)
		ret = new WriteConstruct(arg);
	else {
		po = this->scjit->getElab()->getPort(portAddr);
		ret = new WriteConstruct(po);
		TRACE_3("Port written : " << po->getName() << "\n");
	}

	bool errb = false;
	
	if (po == NULL) {
		ret->setMissingValue(value);
		TRACE_4("Port couldn't be retrieved, setting value to NULL\n");
 	} else if (po->getChannelID() == SIMPLE_CHANNEL) {
		Type* valueType = po->getType();
		std::string chTypeName = po->getChannel()->getTypeName();
		TRACE_4("Attempting to retrieve value of write(), type is : " << chTypeName << "\n");
		if (chTypeName == "N5sc_dt7sc_uintILi8") {
			TRACE_4("handling  uint\n");
			sc_dt::sc_uint<8> intValue = this->scjit->jitType<sc_dt::sc_uint<8> >(fct, callInst, value, &errb);
			if (errb) {
				ret->setMissingValue(value);
				TRACE_3("~~~~~~~~~~~~> OK : setMissingValue on uint\n");
			} else {
				TRACE_4("got result !! \n");				
				sprintf(buffer, "%d", intValue.to_int());
				ret->setValue(buffer);
				TRACE_5("Value written is (sc_uint): " << buffer << "\n");
			}
		} else if (valueType->isIntegerTy()) {
			if (((IntegerType*) valueType)->getBitWidth() == 1) {
				int boolValue = this->scjit->jitBool(fct, callInst, value, &errb);
				if (errb)
					ret->setMissingValue(value);
				else {
					TRACE_5("Value written is (bool): " << boolValue << "\n");
					if (boolValue) {
						ret->setValue("true");
					} else {
						ret->setValue("false");				
					}
				}
			} else {
				int intValue = this->scjit->jitType<int>(fct, callInst, value, &errb);
				if (errb)
					ret->setMissingValue(value);
				else {
					TRACE_5("Value written is (int): " << intValue << "\n");
					sprintf(buffer, "%d", intValue);
					ret->setValue(buffer);
				}
			}
		} else if(valueType->getTypeID() == Type::DoubleTyID) {
			double doubleValue = this->scjit->jitDouble(fct, callInst, value, &errb);
			if (errb)
				ret->setMissingValue(value);
			else {
				TRACE_5("Value written is (double): " << doubleValue << "\n");
				sprintf(buffer, "%f", doubleValue);
				ret->setValue(buffer);
			}
		} else if (valueType->getTypeID() == Type::PointerTyID) {
			ret->setMissingValue(value);
			TRACE_5("Value written is : unknown\n");
		}
	}
	return ret;
}


void WriteHandler::insertInMap(std::map < Function *, SCConstructHandler * >*scchandlers)
{
	SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core8sc_inoutIbE5writeERKb");
	SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core8sc_inoutIiE5writeERKi");
	SCConstructHandler::insertInMap(scchandlers, "_ZN7sc_core8sc_inoutIN5sc_dt7sc_uintILi8EEEE5writeERKS3_");
}
