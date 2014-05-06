/**
 * TwetoPass.cpp
 */


#include "TwetoPass.h"
#include "TwetoPassImpl.h"

char TwetoPass::ID = 0;

using namespace llvm;

TwetoPass::TwetoPass(Frontend * fe, ExecutionEngine * ee,
                     enum tweto_opt_level optimize, bool disableMsg):ModulePass(ID)
{
	impl = new TwetoPassImpl(fe, ee, optimize, disableMsg);
	impl->parent = this;
}

TwetoPass::~TwetoPass()
{
	delete impl;
}

bool TwetoPass::runOnModule(Module & M)
{
	return impl->runOnModule(M);
}

void TwetoPass::getAnalysisUsage (AnalysisUsage& AU) const
{
	AU.addRequiredTransitive<ScalarEvolution>();
}

