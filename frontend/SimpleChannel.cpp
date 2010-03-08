#include <string>
#include <sstream>

#include "SimpleChannel.hpp"

#include "llvm/Type.h"

#include "config.h"

using namespace std;

SimpleChannel::SimpleChannel(Type* globalVariableType, string globalVarTypeName) : Channel(globalVariableType)
{
	this->globalVariableName = "SimpleChannel_" + globalVarTypeName;
	this->id = SIMPLE_CHANNEL;
}

string
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
