#ifndef _DEFAULTTIMECONSTRUCT_HPP
#define _DEFAULTTIMECONSTRUCT_HPP


#include "TimeConstruct.hpp"


struct DefaultTimeConstruct:public TimeConstruct {
	DefaultTimeConstruct(int t):    TimeConstruct::TimeConstruct(t) {}
	DefaultTimeConstruct(double t): TimeConstruct::TimeConstruct(t) {}
	DefaultTimeConstruct(double t, time_unit tu): TimeConstruct::TimeConstruct(t, tu) {}
	std::string toString();
};

#endif
