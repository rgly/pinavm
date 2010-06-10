#include <sstream>
#include <string>
#include "Channel.hpp"
#include "ClockChannel.hpp"

ClockChannel::ClockChannel() : Channel(NULL, NULL)
{
	this->id = CLOCK_CHANNEL;
}

std::string
ClockChannel::toString()
{
	std::stringstream ss;
	ss << "ClockChannel " << (void*) this;
	return ss.str();
}
