#ifndef _EVENTCONSTRUCT_HPP
#define _EVENTCONSTRUCT_HPP

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstruct.hpp"
#include "SCEvent.hpp"

using namespace llvm;
using namespace std;

struct EventConstruct : public SCConstruct {  
protected :
  SCEvent* event;
public:
  EventConstruct(SCEvent* e);
  void setWaitedEvent(SCEvent* e);
  SCEvent* getWaitedEvent();
  string toString();

};

#endif
