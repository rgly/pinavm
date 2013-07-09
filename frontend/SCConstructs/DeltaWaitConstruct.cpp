#include <sstream>
#include "DeltaWaitConstruct.hpp"

DeltaWaitConstruct::DeltaWaitConstruct(int i)
    : WaitConstruct(DELTA), Delta(i)
{}

DeltaWaitConstruct::DeltaWaitConstruct(llvm::Value* missingT)
    : WaitConstruct(DELTA, false), missingDelta(missingT)
{}

int DeltaWaitConstruct::getDelta() const
{
	return this->Delta;
}

llvm::Value* DeltaWaitConstruct::getMissingDelta()
{
	return this->missingDelta;
}

std::string DeltaWaitConstruct::toString()
{
	std::stringstream ss;
	ss << "WAIT(";
	ss << this->getDelta();
	ss << ")";

	return ss.str();
}
