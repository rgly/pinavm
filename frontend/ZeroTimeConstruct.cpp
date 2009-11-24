#include "llvm/Function.h"
#include "llvm/Instructions.h"

#include "ZeroTimeConstruct.hpp"

using namespace llvm;

ZeroTimeConstruct::ZeroTimeConstruct():TimeConstruct(0)
{
}

string ZeroTimeConstruct::toString()
{
	return "WAIT()";
}
