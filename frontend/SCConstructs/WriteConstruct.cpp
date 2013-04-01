#include "WriteConstruct.hpp"


WriteConstruct::WriteConstruct(Port * p)
{
	this->id = WRITECONSTRUCT;
	this->port = p;
	this->missingPort = NULL;
}

WriteConstruct::WriteConstruct(Value * p)
{
	this->id = WRITECONSTRUCT;
	this->missingPort = p;
	this->port = NULL;
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

void
WriteConstruct::setMissingValue(Value* mv)
{
	this->missingValue = mv;
	this->value = "Undefined value, pb in frontend !";
}

void
WriteConstruct::setValue(std::string sv)
{
	this->missingValue = NULL;
	this->value = sv;
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

bool WriteConstruct::isValueDefined()
{
	return (this->missingValue == NULL);
}

bool WriteConstruct::isPortDefined()
{
	return (this->missingPort == NULL);
}
