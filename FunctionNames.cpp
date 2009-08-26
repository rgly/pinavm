//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Streams.h"
#include "llvm/Pass.h"
#include "llvm/Function.h"
#include "llvm/Module.h"
#include "llvm/CallingConv.h"
#include "llvm/DerivedTypes.h"
#include "llvm/InstrTypes.h"
#include "llvm/Instructions.h"

#include "SCJit.hpp"
#include "SCCFactory.hpp"

using namespace llvm;

// Hello - The first implementation, without getAnalysisUsage.
struct FunctionNames : public FunctionPass {
  SCJit* scjit;
  SCCFactory* sccfactory;

public:
  static char ID; // Pass identification, replacement for typeid
  FunctionNames() : FunctionPass(&ID) { }
    
  bool doInitialization(Module& mdl) {
    Module* mod = &mdl;
    this->scjit = new SCJit(mod);
    this->sccfactory = new SCCFactory(scjit);
    
    return true;
  }
  
  bool runOnFunction(Function &F) {
//     llvm::cout << "Function analysed : " << F.getName() << ".\n";
    
    for (Function::iterator bb = F.begin(), be = F.end(); bb != be; ++bb) { 
      llvm::cout << "  --> begin block.\n";
      BasicBlock::iterator i = bb->begin(), ie = bb->end();
      llvm::cout << "size of list = " << bb->getInstList().size() << "\n";
      while (i != ie) {
	llvm::cout << "  --> begin inst.\n";	
	if (CallInst* callInst = dyn_cast<CallInst>(&*i)) {
// 	  llvm::cout << "  try to handle...";
	  sccfactory->handle(&F, &*bb, callInst);
	}
	llvm::cout << " Endings ? \n";
	llvm::cout << "size of list = " << bb->getInstList().size() << "\n";
	BasicBlock::iterator tmpend = bb->end();
	if (tmpend != ie)
	  llvm::cout << " Endings different !\n";
	else
	  llvm::cout << " Endings NOT different !\n";
	i++;
	llvm::cout << "  --> end inst.\n";	
      }
      llvm::cout << "  --> end block.\n";	
    }
     llvm::cout << "end of analysis.\n";
    return false;
  }

  bool doFinalization(Module& mdl) {
    llvm::cout << "-- End of pass.\n";
    this->scjit->doFinalization();
    return true;
  }
};


char FunctionNames::ID = 0;
static RegisterPass<FunctionNames> X("pinapa", "Pinapa pass");
