#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "ZeroTimeConstruct.hpp"

using namespace llvm;

ZeroTimeConstruct::ZeroTimeConstruct() : TimeConstruct(0) {}

char*
ZeroTimeConstruct::toString()
{
  return "WAIT()";
}
