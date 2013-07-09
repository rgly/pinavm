#include "RandConstruct.hpp"

using namespace llvm;

RandConstruct::RandConstruct() : SCConstruct(RANDCONSTRUCT)
{}

std::string RandConstruct::toString()
{
	return "RAND()";
}
