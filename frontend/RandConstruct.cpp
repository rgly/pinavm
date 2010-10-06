#include "RandConstruct.hpp"

using namespace llvm;

RandConstruct::RandConstruct()
{
	this->id = RANDCONSTRUCT;
}

string RandConstruct::toString()
{
	return "RAND()";
}
