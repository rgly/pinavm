/**
 * TwetoPass.h
 *
 * 
 *
 * @author : Si-Mohamed Lamraoui
 * @contact : si-mohamed.lamraoui@imag.fr
 * @date : 2011/03/17
 * @copyright : Verimag 2011
 */

#ifndef TLMBASICPASS_H
#define TLMBASICPASS_H

#include "llvm/Pass.h"
// for enum tweto_opt_level
#include "TwetoBackend.h"

class Frontend;
namespace llvm {
	class ExecutionEngine;
	class ConstantInt;
	class Type;
	class Value;
	class Instruction;
}
class TwetoPassImpl;

// SystemC uses lots of rtti features. Since LLVM library is no-rtti by default,
// we should seperate ModulePass from SystemC library. Or it will results link 
// errors.

// This Class is merely a porxy
class TwetoPass:public llvm::ModulePass {
      private:
	TwetoPassImpl * impl;
      public:
	static char ID;
	TwetoPass(Frontend * fe, llvm::ExecutionEngine * ee,
		  enum tweto_opt_level optimize, bool disableMsg);
	~TwetoPass();
	bool runOnModule(llvm::Module & M);
};


#endif
