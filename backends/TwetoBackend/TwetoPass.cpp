/**
 * TwetoPass.cpp
 */


#include "TwetoPass.h"
#include "TwetoPassImpl.h"

char TwetoPass::ID = 0;

using namespace llvm;

TwetoPass::TwetoPass(Frontend * fe, ExecutionEngine * ee, bool disableMsg):ModulePass
    (ID)
{
	impl = new TwetoPassImpl(fe, ee, disableMsg);
}

TwetoPass::~TwetoPass()
{
	delete impl;
}

bool TwetoPass::runOnModule(Module & M)
{
	return impl->runOnModule(M);
}
