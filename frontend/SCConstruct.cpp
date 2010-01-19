#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstruct.hpp"

using namespace llvm;

SCConstruct::SCConstruct()
{
	this->threadName = "NYI: SCConstruct::threadName";
}

SCConstruct::SCConstruct(Value* missingValue)
{
	this->threadName = "NYI: SCConstruct::threadName";
	this->staticallyFound = false;
	this->dynValue = missingValue;
}

string
SCConstruct::getThreadName()
{
	return this->threadName;
}

construct_id
SCConstruct::getID()
{
	return this->id;
}

Value*
SCConstruct::getMissingValue()
{
	return this->dynValue;
}

bool
SCConstruct::isStaticallyFound()
{
	return this->staticallyFound;
}
