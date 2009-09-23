#ifndef _DEFAULTTIMECONSTRUCT_HPP
#define _DEFAULTTIMECONSTRUCT_HPP

#include <iostream>

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "TimeConstruct.hpp"

using namespace llvm;

struct DefaultTimeConstruct : public TimeConstruct {
  DefaultTimeConstruct(int t) : TimeConstruct::TimeConstruct(t) {}
  string toString();
};

#endif
