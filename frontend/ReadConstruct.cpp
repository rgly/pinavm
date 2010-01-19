#include "ReadConstruct.hpp"
#include <llvm/Value.h>

ReadConstruct::ReadConstruct(Port * p, Value* callInst)
{
	this->id = READCONSTRUCT;
	this->port = p;
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
	return "read(TODO : get value)";
}
