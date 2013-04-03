#ifndef _SCCONSTRUCT_HPP
#define _SCCONSTRUCT_HPP

#include <string>
#include "llvm/Value.h"
//#include <typeinfo>


typedef enum {
	TIMECONSTRUCT,
	WAITEVENTCONSTRUCT,
	NOTIFYCONSTRUCT,
	READCONSTRUCT,
	WRITECONSTRUCT,
	RANDCONSTRUCT,
	ASSERTCONSTRUCT
} construct_id;

struct SCConstruct {
      protected:
	std::string constructName;
	std::string threadName;
	construct_id id;
	bool staticallyFound;
	llvm::Value* dynAddress;

      public:
	SCConstruct();
	SCConstruct(bool found);
	virtual ~SCConstruct(){};
	virtual std::string toString() = 0;
	std::string getThreadName();
	construct_id getID();
	bool isStaticallyFound();
};

#endif
