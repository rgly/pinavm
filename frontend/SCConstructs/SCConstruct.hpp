#ifndef _SCCONSTRUCT_HPP
#define _SCCONSTRUCT_HPP

#include <string>
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
//#include <typeinfo>


typedef enum {
	WAITCONSTRUCT,
	NOTIFYCONSTRUCT,
	READCONSTRUCT,
	WRITECONSTRUCT,
	RANDCONSTRUCT,
	ASSERTCONSTRUCT
} construct_id;

class SCConstruct {
protected:
	// keep it protected so that only sub-class can call this.
	SCConstruct(construct_id);
	SCConstruct(construct_id, bool found);

public:
	virtual ~SCConstruct(){};
	virtual std::string toString() = 0;
	std::string getThreadName();
	construct_id getID() const;
	bool isStaticallyFound();

private:
	construct_id ID;
	bool staticallyFound;
	std::string threadName;
	std::string constructName;
};

#endif
