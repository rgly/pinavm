#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Streams.h"

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "DefaultTimeConstruct.hpp"

using namespace llvm;

char*
DefaultTimeConstruct::toString()
{
  return "WAIT(?)";
}
