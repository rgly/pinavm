#ifndef _FRONTEND_HPP
#define _FRONTEND_HPP

#include <string>

#include "llvm/Module.h"
#include "llvm/Pass.h"

using namespace llvm;

struct SCElab;
struct SCCFactory;
struct SCJit;

extern int launch_systemc(int argc, char *argv[]);

class Frontend {

      private:
	SCJit * scjit;
	SCCFactory *sccfactory;
	SCElab *elab;
	Module *llvmMod;

      public:
	 Frontend(Module * M);
	~Frontend();

	bool run();
	SCElab *getElab();
	void printElab(std::string prefix);
	void printIR();

	Module *getLLVMModule();
	SCCFactory *getConstructs();
};

#endif
