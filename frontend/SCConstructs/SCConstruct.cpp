
#include "SCConstruct.hpp"


SCConstruct::SCConstruct(construct_id id) : ID(id), staticallyFound(true)
{
	this->threadName = "NYI: SCConstruct::threadName";
}

SCConstruct::SCConstruct(construct_id id, bool found) : ID(id), staticallyFound(found)
{
	this->threadName = "NYI: SCConstruct::threadName";
}

std::string
SCConstruct::getThreadName()
{
	return this->threadName;
}

construct_id
SCConstruct::getID() const
{
	return this->ID;
}

bool
SCConstruct::isStaticallyFound()
{
	return this->staticallyFound;
}
