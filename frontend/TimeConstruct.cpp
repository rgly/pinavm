#include "TimeConstruct.hpp"

TimeConstruct::TimeConstruct(int t)
{
  this->time = (double) t;
  this->tu = SC_MS;
}

TimeConstruct::TimeConstruct(double t)
{
  this->time = t;
  this->tu = SC_MS;
}


TimeConstruct::TimeConstruct(double t, time_unit tunit)
{
	this->time = t;
	this->tu = tu;
}

double
TimeConstruct::getTime() {
  return this->time;
}

time_unit TimeConstruct::getTimeUnit()
{
	return this->tu;
}
