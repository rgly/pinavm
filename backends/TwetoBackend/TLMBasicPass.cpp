/**
 * TLMBasicPass.cpp
 */


#include "TLMBasicPass.h"
#include "TLMBasicPassImpl.h"

char TLMBasicPass::ID = 0;

using namespace llvm;

TLMBasicPass::TLMBasicPass(Frontend *fe, ExecutionEngine *ee, bool disableMsg):ModulePass(ID)
{
    impl = new TLMBasicPassImpl(fe, ee, disableMsg);
}

TLMBasicPass::~TLMBasicPass()
{
    delete impl;
}

bool TLMBasicPass::runOnModule(Module &M)
{
    return impl->runOnModule(M);
}


