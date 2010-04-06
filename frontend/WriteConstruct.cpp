#include "WriteConstruct.hpp"


WriteConstruct::WriteConstruct(Port * p, string valueAsString)
{
	this->id = WRITECONSTRUCT;
	this->port = p;
	this->value = valueAsString;
	this->staticallyFound = true;
}

WriteConstruct::WriteConstruct(Port * p, Value* missingValue) : SCConstruct(missingValue)
{
	this->id = WRITECONSTRUCT;
	this->port = p;
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
