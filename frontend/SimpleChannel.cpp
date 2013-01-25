#include <sstream>

#include "SimpleChannel.hpp"

#include "config.h"

SimpleChannel::SimpleChannel(llvm::Type* globalVariableType, std::string globalVarTypeName) : Channel(globalVariableType, globalVarTypeName)
{
	this->globalVariableName = "SimpleChannel_" + globalVarTypeName;
	this->id = SIMPLE_CHANNEL;
}

std::string
SimpleChannel::getGlobalVariableName()
{
	return this->globalVariableName;
}

std::string
SimpleChannel::toString()
{
	std::stringstream ss;
	ss << "SimpleChannel " << (void*) this;
	return ss.str();
}
