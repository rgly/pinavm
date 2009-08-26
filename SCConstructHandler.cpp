#include "SCConstructHandler.hpp"

using namespace llvm;
using namespace std;

SCConstructHandler::SCConstructHandler() {}

SCConstructHandler::SCConstructHandler(SCJit* jit) {
  this->scjit = jit;
}

void SCConstructHandler::insertInMap(std::map<Function*, SCConstructHandler*>* scchandlers, char* fctName)
{
  Function *targetFct = this->scjit->getModule()->getFunction(fctName);
  if (!targetFct) {
    llvm::cout << "Info: targetFct " << fctName << " not found\n" << std::endl;
  } else {
    llvm::cout << "Function found: " << fctName << " Pointer: " << targetFct << std::endl;
    scchandlers->insert(make_pair(targetFct, this));
  }
}
