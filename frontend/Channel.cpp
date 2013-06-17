#include <vector>
#include <algorithm>

#include "Channel.hpp"
#include "Port.hpp"


Channel::Channel(llvm::Type* t, std::string typeName)
{
	this->chTypeName = typeName;
	this->type = t;
	this->id = UNDEFINED_CHANNEL;
	this->ports = new std::vector<Port*>();
}

Channel::~Channel()
{
	delete ports;
}

channel_id
Channel::getID() const
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

void
Channel::addPort(Port* port)
{
	bool IsExisted =
		( std::find(this->ports->begin(), this->ports->end(), port)
		!= this->ports->end() );

	if (IsExisted)
		return;

	this->ports->push_back(port);
}
