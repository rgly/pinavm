#ifndef _EVENT_HPP
#define _EVENT_HPP

#include <string>

#include "IRModule.hpp"
#include "Process.hpp"

using namespace std;

struct IRModule;

struct Event {
protected:
  string eventName;
  Process* process;

public:
  Event(Process* p, string eventName);
  string toString();
  Process* getProcess();
};

#endif
