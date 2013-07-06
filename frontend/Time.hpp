#ifndef PINAVMTIME_HPP
#define PINAVMTIME_HPP

#include "sysc/kernel/sc_time.h"
#include <string>

namespace pinavm
{

class Time
{
public:
	enum TimeUnit {
		ZERO, // In case of Time(zero, TU)
		FS,
		PS,
		NS,
		US,
		MS,
		SEC
	};

	Time(double t, TimeUnit u) : TimeNum(t), TU(u) {};
	Time(double t, sc_core::sc_time_unit sctime);

	// get the time according to the given time unit.
	// Time(0.24, US)::get(NS) == 240
	double get(TimeUnit tu) const;
	double get() const;
	TimeUnit getUnit() const {return this->TU;};
	std::string getUnitStr() const;

	// The unit scale, for example given NS returns 10^-9.
	double getScale(TimeUnit tu) const;

	// For example, Time(52,MS)::convertUnit(US) == Time(52000,US)
	Time convertUnit(TimeUnit tu) const;

	// adjust time unit to make ( 1 <= time < 1000)
	Time adjustUnit() const;
	
private:
	TimeUnit sc2pinavm_time_unit(sc_core::sc_time_unit sc_tu) const;
	TimeUnit lowerUnit(TimeUnit tu) const ;
	TimeUnit higherUnit(TimeUnit tu) const ;

	double TimeNum;
	TimeUnit TU;
};

} // end of namespace.

#endif
