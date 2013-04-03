#include <string>
#include <iostream>
#include "llvm/Module.h"
#include "llvm/Linker.h"
#include "llvm/LLVMContext.h"
#include "llvm/Support/IRReader.h"

#include "GetRootPath.h"

using namespace llvm;

Module* LinkExternalBitcode(Module* module, std::string bc)
{
    LLVMContext &Context = getGlobalContext();
    SMDiagnostic smdiagnostic;
    std::string InputFile = GetRootPath() + "/" + bc;
    Module* LoadBitcode = ParseIRFile(InputFile, smdiagnostic, Context);

    if (!LoadBitcode) {
        std::cerr << InputFile << " : " << smdiagnostic.getMessage() <<'\n';
    }

    std::string err;
    bool result = Linker::LinkModules(module, LoadBitcode,
                         Linker::DestroySource, &err);
    if (result) {
        std::cerr << "Module Linking Error : "<< err << '\n';
    }
    return module;
};
