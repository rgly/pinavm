#ifndef _FRONTENDPASS_HPP
#define _FRONTENDPASS_HPP

#include "llvm/Pass.h"
#include "llvm/Module.h"

struct Frontend;

using namespace llvm;

class FrontendPass:public ModulePass {
      private:
	Frontend * frontend;
	bool inlineFunctions;

      public:
	static char ID;		// Pass identification, replacement for typeid
	FrontendPass();
	Frontend *getFrontend();
	bool runOnModule(Module & M);
	void getAnalysisUsage(AnalysisUsage & AU) const;	
	void setInlineFunctions(bool b);
};

#endif
