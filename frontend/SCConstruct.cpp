#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "SCConstruct.hpp"

using namespace llvm;

SCConstruct::SCConstruct()
{
  this->threadName = "NYI: SCConstruct::threadName";  
}

string
SCConstruct::getThreadName()
{
  return this->threadName;
}
