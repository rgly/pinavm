#include "HelloSCCPrinter.h"
#include "Process.hpp"
#include "HelloConfig.h"

void HelloSCCPrinter::visitTimeWait(TimeWaitConstruct* twc)
{
    _PRINT1(getCurrentProc()->getName());
    _PRINT2(twc->toString());
}

void HelloSCCPrinter::visitEventWait(EventWaitConstruct* ewc)
{
    _PRINT1(getCurrentProc()->getName());
    _PRINT2(ewc->toString());
}

void HelloSCCPrinter::visitDeltaWait(DeltaWaitConstruct* dwc)
{
    _PRINT1(getCurrentProc()->getName());
    _PRINT2(dwc->toString());
}

void HelloSCCPrinter::visitWrite(WriteConstruct* wc)
{
    _PRINT1(getCurrentProc()->getName());
    _PRINT2(wc->toString());
}

void HelloSCCPrinter::visitRead(ReadConstruct* rc)
{
    _PRINT1(getCurrentProc()->getName());
    _PRINT2(rc->toString());
}

void HelloSCCPrinter::visitNotify(NotifyConstruct* nc)
{
    _PRINT1(getCurrentProc()->getName());
    _PRINT2(nc->toString());
}

void HelloSCCPrinter::visitAssert(AssertConstruct* ac)
{
    _PRINT1(getCurrentProc()->getName());
    _PRINT2(ac->toString());
}

void HelloSCCPrinter::visitRand(RandConstruct* rc)
{
    _PRINT1(getCurrentProc()->getName());
    _PRINT2(rc->toString());
}
