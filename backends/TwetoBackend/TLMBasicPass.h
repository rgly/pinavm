/**
 * TLMBasicPass.h
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

struct Frontend;
namespace llvm {
    struct ExecutionEngine;
    struct ConstantInt;
    struct Type;
    struct Value;
    struct Instruction;
}
struct TLMBasicPassImpl;

// Handy structure that keeps the 
// informations for a possible call creation
struct CallInfo {
    // Target module info 
    llvm::ConstantInt *targetModVal;
    const llvm::Type *targetType;
    // Argument info
    llvm::Value *dataArg;
    llvm::Value *addrArg;
    // Call info
    llvm::Instruction *oldcall;
    llvm::Instruction *newcall;
};


// SystemC uses lots of rtti features. Since LLVM library is no-rtti by default,
// we should seperate ModulePass from SystemC library. Or it will results link 
// errors.

// This Class is merely a porxy
class TLMBasicPass : public llvm::ModulePass {
    private:
        TLMBasicPassImpl* impl;
    public:
        static char ID;
        TLMBasicPass(Frontend *fe, llvm::ExecutionEngine *ee, bool disableMsg);
        ~TLMBasicPass();
        bool runOnModule(llvm::Module &M);
};


#endif
