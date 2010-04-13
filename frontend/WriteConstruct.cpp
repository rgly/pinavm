#include "WriteConstruct.hpp"


WriteConstruct::WriteConstruct(Port * p, string valueAsString)
{
	this->id = WRITECONSTRUCT;
	this->port = p;
	this->value = valueAsString;
}

WriteConstruct::WriteConstruct(Port * p, Value* missingV)
{
	this->id = WRITECONSTRUCT;
	this->port = p;
	this->missingValue = missingV;
}

WriteConstruct::WriteConstruct(Value* missingP, Value* missingV) : SCConstruct(false)
{
	this->id = WRITECONSTRUCT;
	this->port = NULL;
	this->missingPort = missingP;
	this->missingValue = missingV;
}


void
 WriteConstruct::setPort(Port * p)
{
	this->port = p;
}

Port *WriteConstruct::getPort()
{
	return this->port;
}

Value *WriteConstruct::getMissingValue()
{
	return this->missingValue;
}

Value *WriteConstruct::getMissingPort()
{
	return this->missingPort;
}

std::string WriteConstruct::toString()
{
	std::string portName;
	if (this->port == NULL)
		portName = "UNKNOWN";
	else
		portName = this->port->getName();

	if (this->value != "")
		return "Write(" + this->value + ") to port : " + portName;
	else
		return "Write(? was not able to jit ?) to port " + portName;
}

std::string WriteConstruct::getValue()
{
	return this->value;
}
