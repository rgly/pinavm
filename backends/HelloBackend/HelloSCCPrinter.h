#ifndef _HELLOSCCPRINTER_H
#define _HELLOSCCPRINTER_H

#include "SCConstructVisitor.h"

class HelloSCCPrinter : public SCConstructVisitor {
public:
    void visitTimeWait(TimeWaitConstruct* twc);
    void visitEventWait(EventWaitConstruct* ewc);
    void visitDeltaWait(DeltaWaitConstruct* dwc);
    void visitWrite(WriteConstruct* wc);
    void visitRead(ReadConstruct* rc);
    void visitNotify(NotifyConstruct* nc);
    void visitAssert(AssertConstruct* ac);
    void visitRand(RandConstruct* rc);
};

#endif
