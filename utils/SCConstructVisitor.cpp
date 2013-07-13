#include "SCConstructVisitor.h"
#include "llvm/Support/ErrorHandling.h"
#include <map>

struct Process;


void SCConstructVisitor::visitSCConstruct(SCConstruct* scc)
{}

void SCConstructVisitor::visitWait(WaitConstruct* wc)
{
    visitSCConstruct(wc);
}

void SCConstructVisitor::visitTimeWait(TimeWaitConstruct* twc)
{
    visitWait(twc);
}

void SCConstructVisitor::visitEventWait(EventWaitConstruct* ewc)
{
    visitWait(ewc);
}
    
void SCConstructVisitor::visitDeltaWait(DeltaWaitConstruct* dwc)
{
    visitWait(dwc);
}

void SCConstructVisitor::visitWrite(WriteConstruct* wc)
{
    visitSCConstruct(wc);
}
    
void SCConstructVisitor::visitRead(ReadConstruct* rc)
{
    visitSCConstruct(rc);
}

void SCConstructVisitor::visitNotify(NotifyConstruct* nc)
{
    visitSCConstruct(nc);
}

void SCConstructVisitor::visitAssert(AssertConstruct* ac)
{
    visitSCConstruct(ac);
}

void SCConstructVisitor::visitRand(RandConstruct* rc)
{
    visitSCConstruct(rc);
}

// if Excluded returns true, then we don't run this scc.
bool SCConstructVisitor::isExcluded(SCConstruct* scc)
{
    return false;
}

void SCConstructVisitor::runOn(SCConstruct* scc)
{
    if (isExcluded(scc))
        return;

    if (isa<WaitConstruct>(scc)) {
        if (isa<TimeWaitConstruct>(scc)) {
            visitTimeWait( dyn_cast<TimeWaitConstruct>(scc));
        } else if (isa<EventWaitConstruct>(scc)) {
            visitEventWait( dyn_cast<EventWaitConstruct>(scc));
        } else if (isa<DeltaWaitConstruct>(scc)) {
            visitDeltaWait( dyn_cast<DeltaWaitConstruct>(scc));
        }
    } else if (isa<WriteConstruct>(scc)) {
        visitWrite( dyn_cast<WriteConstruct>(scc));
    } else if (isa<ReadConstruct>(scc)) {
        visitRead( dyn_cast<ReadConstruct>(scc));
    } else if (isa<NotifyConstruct>(scc)) {
        visitNotify( dyn_cast<NotifyConstruct>(scc));
    } else if (isa<AssertConstruct>(scc)) {
        visitAssert( dyn_cast<AssertConstruct>(scc));
    } else if (isa<RandConstruct>(scc)) {
        visitRand( dyn_cast<RandConstruct>(scc));
    } else {
        llvm_unreachable("Unknown Construct Type");
    }
}

void SCConstructVisitor::runOn(SCCFactory* factory)
{
    std::map<Instruction*, std::map<Process*, SCConstruct*> >* constructs
        = factory->getConstructs();

    std::map<Instruction*, std::map<Process*, SCConstruct*> >::iterator it, ie;
    it = constructs->begin();
    ie = constructs->end();

    for(;it != ie; ++it) {
        std::map<Process*, SCConstruct*>* p2sccMap = &(it->second);
        std::map<Process*, SCConstruct*>::iterator iit, iie;
        iit = p2sccMap->begin();
        iie = p2sccMap->end();

        for(;iit != iie; ++iit) {
            SCConstruct* scc = iit->second;
            if (!isExcluded(scc))
                runOn(scc);
        }
    }
}
