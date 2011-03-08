#include <sstream>
#include <string>
#include "Channel.hpp"
#include "BasicChannel.hpp"

BasicChannel::BasicChannel() : Channel(NULL, "")
{
	this->id = BASIC_CHANNEL;
}

std::string
BasicChannel::toString()
{
	std::stringstream ss;
	ss << "BasicChannel " << (void*) this;
	return ss.str();
}
