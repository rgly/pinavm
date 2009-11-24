#ifndef _CONFIG_H
#define _CONFIG_H

#include "llvm/Support/Streams.h"

#define VERBOSE 7

#if VERBOSE >= 1
#define TRACE_1(Msg) llvm::cout << "###> " << Msg
#else
#define TRACE_1(Msg)
#endif

#if VERBOSE >= 2
#define TRACE_2(Msg) llvm::cout << "   **> " << Msg
#else
#define TRACE_2(Msg)
#endif

#if VERBOSE >= 3
#define TRACE_3(Msg) llvm::cout << "      -> " << Msg
#else
#define TRACE_3(Msg)
#endif

#if VERBOSE >= 4
#define TRACE_4(Msg) llvm::cout << "           " << Msg
#else
#define TRACE_4(Msg)
#endif

#if VERBOSE >= 5
#define TRACE_5(Msg) llvm::cout << "              " << Msg
#else
#define TRACE_5(Msg)
#endif

#if VERBOSE >= 6
#define TRACE_6(Msg) llvm::cout << "                 " << Msg
#else
#define TRACE_6(Msg)
#endif


#if VERBOSE >= 5
#define PRINT_5(Inst) Inst
#else
#define PRINT_5(Inst)
#endif

#if VERBOSE >= 6
#define PRINT_6(Inst) Inst
#else
#define PRINT_6(Inst)
#endif


#define ERROR(Msg)    llvm::cout << Msg << "\n\n"; exit(1);


#endif
