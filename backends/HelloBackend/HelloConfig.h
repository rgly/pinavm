#ifndef _HELLO_CONFIG_H
#define _HELLO_CONFIG_H

#include "llvm/Support/raw_ostream.h"

#define _PRINT(a) llvm::outs() << a << '\n'
#define _PRINT1(a) _PRINT("\t" << a )
#define _PRINT2(a) _PRINT1("\t" << a)

#endif
