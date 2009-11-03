#ifndef _NOTIFYCONSTRUCT_HPP
#define _NOTIFYCONSTRUCT_HPP

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstruct.hpp"
#include "SCEvent.hpp"

using namespace llvm;

struct NotifyConstruct : public SCConstruct {  

protected :
  SCEvent* event;
public:
  NotifyConstruct(SCEvent* e);
  void setNotifiedEvent(SCEvent* e);
  SCEvent* getNotifiedEvent();
  string toString();
};

#endif
