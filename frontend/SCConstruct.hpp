#ifndef _SCCONSTRUCT_HPP
#define _SCCONSTRUCT_HPP

#include <string>

#include "llvm/Instruction.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include "llvm/Value.h"

using namespace llvm;
using namespace std;

typedef enum {
	TIMECONSTRUCT,
	WAITEVENTCONSTRUCT,
	NOTIFYCONSTRUCT,
	READCONSTRUCT,
	WRITECONSTRUCT
} construct_id;

struct SCConstruct {
      protected:
	string constructName;
	string threadName;
	construct_id id;
	bool staticallyFound;
	Value* dynValue;

      public:
	SCConstruct();
	SCConstruct(Value* missingValue);
	virtual string toString() = 0;
	string getThreadName();
	construct_id getID();
	Value* getMissingValue();
	bool isStaticallyFound();
};

#endif
