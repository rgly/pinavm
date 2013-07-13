#include "ALLConstruct.h"
#include "SCCFactory.hpp"

class SCConstructVisitor {
public:
    virtual void visitSCConstruct(SCConstruct* scc);
    virtual void visitWait(WaitConstruct* wc);
    virtual void visitTimeWait(TimeWaitConstruct* twc);
    virtual void visitEventWait(EventWaitConstruct* ewc);
    virtual void visitDeltaWait(DeltaWaitConstruct* dwc);
    virtual void visitWrite(WriteConstruct* wc);
    virtual void visitRead(ReadConstruct* rc);
    virtual void visitNotify(NotifyConstruct* nc);
    virtual void visitAssert(AssertConstruct* ac);
    virtual void visitRand(RandConstruct* rc);

    // if Exclusive returns true, then we don't run this scc.
    virtual bool isExcluded(SCConstruct* scc);
    void runOn(SCConstruct* scc);
    void runOn(SCCFactory* factory);
};
