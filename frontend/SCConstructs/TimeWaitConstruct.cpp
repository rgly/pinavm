#include <sstream>
#include "TimeWaitConstruct.hpp"

TimeWaitConstruct::TimeWaitConstruct(llvm::Value* missingT)
    : WaitConstruct(TIMED, false), time(0, pinavm::Time::ZERO), missingTime(missingT)
{}

TimeWaitConstruct::TimeWaitConstruct(pinavm::Time t)
    : WaitConstruct(TIMED), time(t)
{}

pinavm::Time TimeWaitConstruct::getTime() const
{
	return this->time;
}

llvm::Value* TimeWaitConstruct::getMissingTime()
{
	return this->missingTime;
}

std::string TimeWaitConstruct::toString()
{
	std::stringstream ss;
	ss << "WAIT(";
	ss << this->getTime().get();
	ss << ", ";
	ss << this->getTime().getUnitStr();
	ss << ")";

	return ss.str();
}
