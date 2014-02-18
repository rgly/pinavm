#include "ALLConstruct.h"
#include "SCCFactory.hpp"

namespace llvm
{
    struct Instruction;
}

struct Process;

class SCConstructVisitor {
private:
    llvm::Instruction* CurrentInst;
    Process* CurrentProc;

protected:
    // visitXXX now can access Inst and Process with these getters.
    // Beware, these function only valid when runOn_Factory is being called.
    llvm::Instruction* getCurrentInst();
    Process* getCurrentProc();

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
