#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Streams.h"

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "TimeHandler.hpp"

using namespace llvm;

int
TimeHandler::getTime()
{
  return this->time;
}
