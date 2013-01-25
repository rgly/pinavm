#include "AssertConstruct.hpp"

using namespace llvm;

AssertConstruct::AssertConstruct(bool b)
{
	this->cond = b;
	this->id = ASSERTCONSTRUCT;
}

AssertConstruct::AssertConstruct(Value * missingC) : SCConstruct(false)
{
	this->cond = NULL;
	this->missingCond = missingC;
	this->id = ASSERTCONSTRUCT;
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
	if (this->staticallyFound)
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
