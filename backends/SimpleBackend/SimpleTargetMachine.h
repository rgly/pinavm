#ifndef SIMPLETARGETMACHINE_H
#define SIMPLETARGETMACHINE_H


#include "llvm/Target/TargetMachine.h"
#include "llvm/DataLayout.h"
#include "llvm/PassManager.h"

//#include "SimpleBackendNameAllUsedStructsAndMergeFunctions.h"

namespace llvm {
  
  struct SimpleTargetMachine : public TargetMachine {
  SimpleTargetMachine(const Target &T, StringRef TargetTriple, StringRef CPU,
                    StringRef FS, const TargetOptions &Options, Reloc::Model RM,
                    CodeModel::Model CM, CodeGenOpt::Level OL)
    : TargetMachine(T, TargetTriple, CPU, FS, Options ) {}
    
    virtual bool WantsWholeFile() const {
      return true;
    }
    virtual bool addPassesToEmitWholeFile(PassManager &PM,
					  formatted_raw_ostream &Out,
					  CodeGenFileType FileType,
					  CodeGenOpt::Level OptLevel,
					  bool DisableVerify = true);

    virtual const DataLayout*
    getDataLayout() const {
      return 0;
    }

  };
  
  extern Target TheSimpleBackendTarget;
  
} // End llvm namespace


#endif
