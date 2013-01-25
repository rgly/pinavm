#include <vector>

#include "Channel.hpp"


Channel::Channel(llvm::Type* t, std::string typeName)
{
	this->chTypeName = typeName;
	this->type = t;
	this->id = UNDEFINED_CHANNEL;
	this->ports = new std::vector<Port*>();
}

channel_id
Channel::getID()
{
	return this->id;
}

llvm::Type*
Channel::getType()
{
	return this->type;
}

std::string
Channel::getTypeName()
{
	return this->chTypeName;
}

std::vector<Port*>*
Channel::getPorts()
{
	return this->ports;
}
