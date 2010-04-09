#include <string>
#include <sstream>

#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"

#include "llvm/Support/raw_ostream.h"

using namespace llvm;

std::stringstream ErrorMsg("");

void
triggerError(formatted_raw_ostream & Out)
{
	Out.flush();
	delete &Out;
	report_fatal_error(ErrorMsg.str());
}

void triggerError(formatted_raw_ostream & Out, std::string msg)
{
	ErrorMsg << msg;
	triggerError(Out);
}

std::string
intToString(int anInt)
{
	std::stringstream ss;
	ss << anInt;
	return ss.str();
}

std::string&
replaceAll(std::string& context, const std::string& from, const std::string& to) {
	size_t lookHere = 0;
	size_t foundHere;
	while((foundHere = context.find(from, lookHere)) != std::string::npos) {
		context.replace(foundHere, from.size(), to);
		lookHere = foundHere + to.size();
	}
	return context;
} ///:~
