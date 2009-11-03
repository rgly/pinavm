#ifndef _SCEVENT_HPP
#define _SCEVENT_HPP

#include <string>

using namespace std;

struct SCEvent {
protected:
  string eventName;

public:
  SCEvent(string eName);
  string toString();
};

#endif
