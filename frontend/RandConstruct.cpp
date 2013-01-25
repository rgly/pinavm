#include "RandConstruct.hpp"

using namespace llvm;

RandConstruct::RandConstruct()
{
	this->id = RANDCONSTRUCT;
}

std::string RandConstruct::toString()
{
	return "RAND()";
}
