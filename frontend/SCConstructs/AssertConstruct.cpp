#include "AssertConstruct.hpp"

using namespace llvm;

AssertConstruct::AssertConstruct(bool b) : SCConstruct(ASSERTCONSTRUCT)
{
	this->cond = b;
}

AssertConstruct::AssertConstruct(Value * missingC)
	: SCConstruct(ASSERTCONSTRUCT, false)
{
	this->cond = false;
	this->missingCond = missingC;
}

void
AssertConstruct::setCond(bool b)
{
	this->cond = b;
}

bool AssertConstruct::getCond()
{
	return this->cond;
}

std::string AssertConstruct::toString()
{
	if (this->isStaticallyFound())
		if (this->cond)
			return "ASSERT(true)";
		else
			return "ASSERT(false)";			
	else
		return "ASSERT(dynamic content)";
}

Value*
AssertConstruct::getMissingCond()
{
	return this->missingCond;
}
