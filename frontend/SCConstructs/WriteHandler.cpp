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

	void *portAddr = this->scjit->jitType<void*>(fct, callInst, arg, NULL);
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
			sc_dt::sc_uint<8>* pintValue = this->scjit->jitType<sc_dt::sc_uint<8>* >(fct, callInst, value, &errb);
			if (errb) {
				ret->setMissingValue(value);
				TRACE_3("~~~~~~~~~~~~> OK : setMissingValue on uint\n");
			} else {
				TRACE_4("got result !! \n");				
				sprintf(buffer, "%d", pintValue->to_int());
				ret->setValue(buffer);
				TRACE_5("Value written is (sc_uint): " << buffer << "\n");
			}
		} else if (valueType->isIntegerTy()) {
			if (((IntegerType*) valueType)->getBitWidth() == 1) {
				bool* pboolValue = this->scjit->jitType<bool*>(fct, callInst, value, &errb);
				if (errb)
					ret->setMissingValue(value);
				else {
					TRACE_5("Value written is (bool): " << *pboolValue << "\n");
					if (*pboolValue) {
						ret->setValue("true");
					} else {
						ret->setValue("false");				
					}
				}
			} else {
				int* pintValue = this->scjit->jitType<int*>(fct, callInst, value, &errb);
				if (errb)
					ret->setMissingValue(value);
				else {
					TRACE_5("Value written is (int): " << *pintValue << "\n");
					sprintf(buffer, "%d", *pintValue);
					ret->setValue(buffer);
				}
			}
		} else if(valueType->getTypeID() == Type::DoubleTyID) {
			double* pdoubleValue = this->scjit->jitType<double*>(fct, callInst, value, &errb);
			if (errb)
				ret->setMissingValue(value);
			else {
				TRACE_5("Value written is (double): " << pdoubleValue << "\n");
				sprintf(buffer, "%f", pdoubleValue);
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
