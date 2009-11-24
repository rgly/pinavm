#ifndef _PORT_HPP
#define _PORT_HPP

#include <string>

#include "IRModule.hpp"

using namespace std;
using namespace llvm;

struct IRModule;

struct Port {
private:
  IRModule* irModule;  
  string name;
  
public:
  Port(IRModule* module, string portName);
  IRModule* getModule();
  string getName();
};

#endif
