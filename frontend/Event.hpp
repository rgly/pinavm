#ifndef _EVENT_HPP
#define _EVENT_HPP

#include <string>
#include <vector>

#include "ElabMember.hpp"


struct Process;

struct Event:public ElabMember {
protected:
  std::string eventName;
  std::vector<Process*> processes;
  int numEvent;

public:
  Event(std::string name);
  std::string getEventName();
  std::vector<Process*>* getProcesses();
  void printElab(int sep, std::string prefix);
  int getNumEvent();
  void addProcess(Process* p);
};

#endif
