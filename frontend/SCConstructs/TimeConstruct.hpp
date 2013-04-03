#ifndef _TIMECONSTRUCT_HPP
#define _TIMECONSTRUCT_HPP

#include "llvm/Value.h"

#include "SCConstruct.hpp"


typedef enum e_time_unit {
	SC_FS = 0,
	SC_PS,
	SC_NS,
	SC_US,
	SC_MS,
	SC_SEC
} time_unit;


struct TimeConstruct:public SCConstruct {
protected:
	double time;
	time_unit tu;
	llvm::Value* missingTime;
public:
	TimeConstruct(llvm::Value* missingT);
	TimeConstruct(int t);
	TimeConstruct(double t);
	TimeConstruct(double t, time_unit tunit);
	double getTime();
	time_unit getTimeUnit();
	llvm::Value* getMissingTime();
};

#endif
