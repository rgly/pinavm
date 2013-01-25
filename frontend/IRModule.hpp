#ifndef _IRMODULE_HPP
#define _IRMODULE_HPP

#include <string>
#include <vector>

#include "llvm/GlobalVariable.h"
#include "llvm/Function.h"

#include "ElabMember.hpp"

using namespace llvm;

struct Port;
struct Event;
struct Process;

struct IRModule:public ElabMember {
private:
  std::string name;
  std::string typeName;
  std::vector < Process * >processes;
  std::vector < Port * >ports;
  std::vector < Event * >events;
  std::vector < GlobalVariable * >sharedVariables;

public:
  IRModule(std::string typeName, std::string moduleName);
  std::vector < Process * >*getProcesses();
  void addProcess(Process * process);
  std::vector < Port * >*getPorts();
  void addPort(Port * port);

  std::vector < Event * >*getEvents();
  void addEvent(Event * event);

  std::vector < GlobalVariable * >*getSharedVariables();
  void addSharedVariable(GlobalVariable * gv);

  std::string getModuleType();
  std::string getUniqueName();

  void printElab(int sep, std::string prefix);
};

#endif
