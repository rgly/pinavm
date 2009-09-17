#ifndef _EVENT_HPP
#define _EVENT_HPP

#include "llvm/Function.h"
#include "llvm/Instructions.h"

using namespace llvm;

struct Event {
protected:
  char* eventName;

public:
  Event(char* eName);
  char* toString();
};

#endif
