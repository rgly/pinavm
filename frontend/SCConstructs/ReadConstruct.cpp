#include "ReadConstruct.hpp"
#include "Port.hpp"

#include "llvm/IR/Value.h"

ReadConstruct::ReadConstruct(Port * p, Value* callInst)
	: SCConstruct(READCONSTRUCT)
{
	this->port = p;
	this->callInstruction = callInst;
}

ReadConstruct::ReadConstruct(Value* callInst) : SCConstruct(READCONSTRUCT,false)
{
	this->port = NULL;
	this->callInstruction = callInst;
}

Value*
ReadConstruct::getCallInst()
{
	return this->callInstruction;
}

void
ReadConstruct::setPort(Port * p)
{
	this->port = p;
}

Port *ReadConstruct::getPort()
{
	return this->port;
}

std::string ReadConstruct::toString()
{
	std::string portName;
	if (this->port == nullptr)
		portName = "UNKNOWN";
	else
		portName = this->port->getName();

	return "Read() in port : " + portName;
}
