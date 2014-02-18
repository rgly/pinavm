// This header records some functions which I can not decide how to deal
// them when porting PinaVM to newer version.

#include "llvm/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"

namespace pinavm
{
// CW Chang: GCInfoDeleter has merged into llvm::Printer::doFinalization(M)
//           Since llvm 3.3, I can't decide using
//           createGCInfoPrinter(raw_ostream&) instead or not.
//           Currently comment-out GCInfoDeleter.
void addGCInfoDeleter(llvm::PassManager& Passes);
}// end of namespace pinavm
