#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

extern std::stringstream ErrorMsg;

std::string& replaceAll(std::string& context, const std::string& from, const std::string& to);
std::string intToString(int anInt);
void triggerError(formatted_raw_ostream & Out);
void triggerError(formatted_raw_ostream & Out, std::string msg);

#endif
