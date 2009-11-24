#ifndef _TIMECONSTRUCT_HPP
#define _TIMECONSTRUCT_HPP

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstruct.hpp"

using namespace llvm;

typedef enum e_time_unit {
	SC_FS = 0,
	SC_PS = 1,
	SC_NS = 2,
	SC_MS = 3,
	SC_SEC = 4
} time_unit;


struct TimeConstruct : public SCConstruct {  
protected :
  double time;
  time_unit tu;
  
public:
  TimeConstruct(int t);
  TimeConstruct(double t);
  TimeConstruct(double t, time_unit tunit);
  double getTime();
  time_unit getTimeUnit();
};

#endif
