#include "llvm/Pass.h"
#include "llvm/Module.h"

#include "Frontend.hpp"
#include "FrontendPass.hpp"

using namespace llvm;

FrontendPass::FrontendPass():ModulePass(ID)
{
	this->inlineFunctions = false;
}

void
FrontendPass::setInlineFunctions(bool b)
{
	this->inlineFunctions = b;
}

bool FrontendPass::runOnModule(Module & M)
{
	this->frontend = new Frontend(&M);
	this->frontend->setInlineFunctions(this->inlineFunctions);
	return this->frontend->run();

}

Frontend *FrontendPass::getFrontend()
{
	return this->frontend;
}

void FrontendPass::getAnalysisUsage(AnalysisUsage & AU) const
{
	AU.setPreservesAll();
}

char FrontendPass::ID = 42;
static RegisterPass < FrontendPass > X("pinavm", "PinaVM pass");
