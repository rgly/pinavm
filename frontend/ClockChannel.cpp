#include <sstream>
#include <string>
#include "Channel.hpp"
#include "ClockChannel.hpp"

ClockChannel::ClockChannel() : Channel(NULL, "")
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

void ClockChannel::setClock(double period_, double duty_cycle_,
                            double start_time_, bool posedge_first_)
{
        this->period = period_;
        this->duty_cycle = duty_cycle_;
        this->start_time = start_time_;
        this->posedge_first = posedge_first_;
}
