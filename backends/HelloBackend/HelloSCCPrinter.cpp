#include "HelloSCCPrinter.h"
#include <iostream>

#define _PRINT(a) std::cout << a << '\n'

void HelloSCCPrinter::visitTimeWait(TimeWaitConstruct* twc)
{
    _PRINT(twc->toString());
}

void HelloSCCPrinter::visitEventWait(EventWaitConstruct* ewc)
{
    _PRINT(ewc->toString());
}

void HelloSCCPrinter::visitDeltaWait(DeltaWaitConstruct* dwc)
{
    _PRINT(dwc->toString());
}

void HelloSCCPrinter::visitWrite(WriteConstruct* wc)
{
    _PRINT(wc->toString());
}

void HelloSCCPrinter::visitRead(ReadConstruct* rc)
{
    _PRINT(rc->toString());
}

void HelloSCCPrinter::visitNotify(NotifyConstruct* nc)
{
    _PRINT(nc->toString());
}

void HelloSCCPrinter::visitAssert(AssertConstruct* ac)
{
    _PRINT(ac->toString());
}

void HelloSCCPrinter::visitRand(RandConstruct* rc)
{
    _PRINT(rc->toString());
}
