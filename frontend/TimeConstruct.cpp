#include "TimeConstruct.hpp"

TimeConstruct::TimeConstruct(llvm::Value* missingT) : SCConstruct(false)
{
	this->id = TIMECONSTRUCT;
	this->missingTime = missingT;
}

TimeConstruct::TimeConstruct(int t)
{
	this->id = TIMECONSTRUCT;
	this->time = (double) t;
	this->tu = SC_MS;
}

TimeConstruct::TimeConstruct(double t)
{
	this->id = TIMECONSTRUCT;
	this->time = t;
	this->tu = SC_MS;
}


TimeConstruct::TimeConstruct(double t, time_unit tunit)
{
	this->id = TIMECONSTRUCT;
	this->time = t;
	this->tu = tunit;
}

double
 TimeConstruct::getTime()
{
	return this->time;
}

time_unit TimeConstruct::getTimeUnit()
{
	return this->tu;
}
