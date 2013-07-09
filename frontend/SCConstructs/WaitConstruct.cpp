#include "WaitConstruct.hpp"


WaitConstruct::WaitConstruct(WaitIDTy id) : SCConstruct(WAITCONSTRUCT), WaitID(id)
{}

WaitConstruct::WaitConstruct(WaitIDTy id, bool b) : SCConstruct(WAITCONSTRUCT, b), WaitID(id)
{}

WaitIDTy WaitConstruct::getWaitID() const
{
    return this->WaitID;
}
