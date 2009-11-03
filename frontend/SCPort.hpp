#ifndef _SCPORT_HPP
#define _SCPORT_HPP

#include <string>

#include "SCModule.hpp"

using namespace std;
using namespace llvm;

struct SCModule;

struct SCPort {
private:
  SCModule* scModule;
  SCModule* bindedSCModule;
  
  string name;
  
public:
  SCPort(SCModule* module, string portName);
  SCModule* getSCModule();
  SCModule* getBindedModule();
  void setBindedModule(SCModule* mod);
  string getPortName();
};

#endif
