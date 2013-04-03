#ifndef _CLOCKCHANNEL_HPP
#define _CLOCKCHANNEL_HPP

#include <string>
#include "Channel.hpp"

class ClockChannel : public Channel
{
public:
	ClockChannel();
	std::string toString();
        void setClock(double,double,double,bool) ;
        double getPeriod(){return this->period ;};
        double getStartTime(){return this->start_time;} ;
        double getDutyCycle(){return this->duty_cycle ;} ;
        bool getPosedgeFirst(){return this->posedge_first;};
private:
        // the duration of one clock cycle.
        double period ;
        // the time of when to start.
        double start_time ;
        // the percentage of negetive.
        double duty_cycle ;
        bool posedge_first ;
};

#endif
