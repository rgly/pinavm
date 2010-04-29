#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstruct.hpp"

using namespace llvm;

SCConstruct::SCConstruct()
{
	this->threadName = "NYI: SCConstruct::threadName";
	this->staticallyFound = true;
}

SCConstruct::SCConstruct(bool found)
{
	this->threadName = "NYI: SCConstruct::threadName";
	this->staticallyFound = found;
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

bool
SCConstruct::isStaticallyFound()
{
	return this->staticallyFound;
}
