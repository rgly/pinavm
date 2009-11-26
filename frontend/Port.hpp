#ifndef _PORT_HPP
#define _PORT_HPP

#include <string>

#include "ElabMember.hpp"

using namespace std;
using namespace llvm;

struct IRModule;

struct Port : public ElabMember {
private:
  IRModule* irModule;  
  string name;
  
public:
  Port(IRModule* module, string portName);
  IRModule* getModule();
  string getName();
  void printElab(int sep, string prefix);
};

#endif
