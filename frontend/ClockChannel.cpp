#include "Channel.hpp"
#include "ClockChannel.hpp"

ClockChannel::ClockChannel() : Channel(NULL)
{
	this->id = CLOCK_CHANNEL;
}
