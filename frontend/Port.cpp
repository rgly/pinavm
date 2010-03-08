#include <string>
#include <vector>

#include "IRModule.hpp"
#include "Port.hpp"
#include "Channel.hpp"

#include "llvm/Type.h"

#include "config.h"

using namespace std;

Port::Port(IRModule * module, string portName)
{
	this->irModule = module;
	this->name = portName;
	this->channels = new vector<Channel*>();
	this->channelID = UNDEFINED_CHANNEL;
}

IRModule *Port::getModule()
{
	return this->irModule;
}

string Port::getName()
{
	return this->name;
}

vector<Channel*>*
Port::getChannels()
{
	return this->channels;
}

Channel*
Port::getChannel()
{
	return this->channel;
}

void
Port::addChannel(Channel* ch)
{
	if (find(this->channels->begin(), this->channels->end(), ch) != this->channels->end()) {
		ERROR("Channel added twice to the same port : " << ch << "\n");
	} else if (this->channelID == UNDEFINED_CHANNEL) {
		this->channelID = ch->getID();
		this->type = ch->getType();
		this->channel = ch;
		TRACE_4("Add channel\n");
	} else if (this->channelID != ch->getID()) {
		ERROR("Cannot add different types of Channel in the same port\n");
	}
	this->channels->push_back(ch);
}

channel_id
Port::getChannelID()
{
	return this->channelID;
}

Type*
Port::getType()
{
	return this->type;
}

/********** Pretty print **********/
void Port::printElab(int sep, string prefix)
{
	this->printPrefix(sep, prefix);
	std::string chstr = this->channel->toString();
	TRACE("Port : " << this->getName() << " (\"" << (void*) this << "\"), bounded to channel " << chstr << "\n");
}
