#include "llvm/Support/ErrorHandling.h"

#include "Time.hpp"


namespace pinavm
{

Time::Time(double t, TimeUnit u) : TimeNum(t)
{
	if (t == 0)
		this->TU = ZERO;
	else
		this->TU = u;
}

Time::Time(double t, sc_core::sc_time_unit sctime) : TimeNum(t)
{
	if (t == 0)
		this->TU = ZERO;
	else
		this->TU = this->sc2pinavm_time_unit(sctime);
}

double Time::get(TimeUnit tu) const
{
	return this->convertUnit(tu).get();
}

double Time::get() const
{
	return this->TimeNum;
}

std::string Time::getUnitStr() const
{
	TimeUnit tu = this->getUnit();
	switch (tu) {
		// Returning merely ZERO is hard to identify it as a time unit. 
		case ZERO: return "ZERO_TIME";
		case FS: return "FS";
		case PS: return "PS";
		case NS: return "NS";
		case US: return "US";
		case MS: return "MS";
		case SEC: return "SEC";
		default: llvm_unreachable("Unknown Time Unit");
	}
}

bool Time::isZero() const
{
	return (this->getUnit() == ZERO);
}

double Time::getScale(TimeUnit tu) const
{
	const double scale = 0.001;
	double ret = 1;
	switch (tu) {
		case ZERO: ret = 0; break;
		case FS: ret *= scale;
		case PS: ret *= scale;
		case NS: ret *= scale;
		case US: ret *= scale;
		case MS: ret *= scale;
		case SEC: ret *= 1; break;
		default: llvm_unreachable("Unknown Time Unit");
	}

	return ret;
}

Time Time::convertUnit(TimeUnit tu) const
{
	double scale = this->getScale(this->getUnit()) / this->getScale(tu);
	return Time(this->get() * scale, tu);
}

Time Time::adjustUnit() const
{
	const double scale = 1000;
	double t = this->get();
	TimeUnit u = this->getUnit();

	if (u == ZERO)
		return *this;

	while (1) {
		if (t < 1 && u != FS) {
			t *= scale;
			u = this->lowerUnit(u);
		} else if (t >= 1000 && u != SEC) {
			t /= scale;
			u = this->higherUnit(u);
		} else
			break;
	}

	return Time(t,u);
}

Time::TimeUnit Time::sc2pinavm_time_unit(sc_core::sc_time_unit sc_tu) const
{
	switch (sc_tu) {
		case sc_core::SC_FS : return FS;
		case sc_core::SC_PS : return PS;
		case sc_core::SC_NS : return NS;
		case sc_core::SC_US : return US;
		case sc_core::SC_MS : return MS;
		case sc_core::SC_SEC : return SEC;
		default: llvm_unreachable("Unknown SC_Time Unit");
	}
}

Time::TimeUnit Time::lowerUnit(TimeUnit tu) const
{
	switch (tu) {
		case PS: return FS;
		case NS: return PS;
		case US: return NS;
		case MS: return US;
		case SEC: return MS;
		case FS: 
		default: llvm_unreachable("Can not lower FS");
	}
}

Time::TimeUnit Time::higherUnit(TimeUnit tu) const
{
	switch (tu) {
		case FS: return PS;
		case PS: return NS;
		case NS: return US;
		case US: return MS;
		case MS: return SEC;
		case SEC: 
		default: llvm_unreachable("Can not higher SEC");
	}
}

} // end of pinavm namespace
