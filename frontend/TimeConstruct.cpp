#include "TimeConstruct.hpp"

TimeConstruct::TimeConstruct(int t)
{
	this->time = t;
	this->tu = SC_MS;
}

TimeConstruct::TimeConstruct(int t, time_unit tunit)
{
	this->time = t;
	this->tu = tu;
}

int
 TimeConstruct::getTime()
{
	return this->time;
}

time_unit TimeConstruct::getTimeUnit()
{
	return this->tu;
}
