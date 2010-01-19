#ifndef _SIMPLECHANNEL_HPP
#define _SIMPLECHANNEL_HPP

#include <string>

#include "Channel.hpp"

#include <llvm/Type.h>

using namespace llvm;

class SimpleChannel : public Channel
{
protected:
	std::string globalVariableName;
public:
	SimpleChannel(Type* globalVariableType, std::string globalVarTypeName);
	std::string getGlobalVariableName();
};

#endif
