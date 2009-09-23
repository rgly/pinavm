#ifndef _EVENT_HPP
#define _EVENT_HPP

#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include <string>

using namespace llvm;
using namespace std;

struct Event {
protected:
  string eventName;

public:
  Event(string eName);
  string toString();
};

#endif
