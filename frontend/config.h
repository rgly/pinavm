#ifndef _CONFIG_H
#define _CONFIG_H

#include <iostream>
#include <unistd.h>
#include <stdlib.h>

//#include "llvm/Support/Streams.h"

extern bool disable_debug_msg;

#define VERBOSE 7

#define TRACE(Msg) std::cout << Msg; std::cout.flush();

#define TRACE_DEBUG(Msg) if (! disable_debug_msg) { TRACE(Msg) }

#if VERBOSE >= 1
#define TRACE_1(Msg) TRACE_DEBUG("###> " << Msg)
#else
#define TRACE_1(Msg)
#endif

#if VERBOSE >= 2
#define TRACE_2(Msg) TRACE_DEBUG("   **> " << Msg)
#else
#define TRACE_2(Msg)
#endif

#if VERBOSE >= 3
#define TRACE_3(Msg) TRACE_DEBUG("      -> " << Msg)
#else
#define TRACE_3(Msg)
#endif

#if VERBOSE >= 4
#define TRACE_4(Msg) TRACE_DEBUG("           " << Msg)
#else
#define TRACE_4(Msg)
#endif

#if VERBOSE >= 5
#define TRACE_5(Msg) TRACE_DEBUG("              " << Msg)
#else
#define TRACE_5(Msg)
#endif

#if VERBOSE >= 6
#define TRACE_6(Msg) TRACE_DEBUG("                 " << Msg)
#else
#define TRACE_6(Msg)
#endif

#if VERBOSE >= 7
#define TRACE_7(Msg) TRACE_DEBUG("                 " << Msg)
#else
#define TRACE_7(Msg)
#endif



#if VERBOSE >= 3 
#define PRINT_3(Inst) if (! disable_debug_msg) {Inst;}
#else
#define PRINT_3(Inst)
#endif

#if VERBOSE >= 4 
#define PRINT_4(Inst) if (! disable_debug_msg) {Inst;}
#else
#define PRINT_4(Inst)
#endif

#if VERBOSE >= 5 
#define PRINT_5(Inst) if (! disable_debug_msg) {Inst;}
#else
#define PRINT_5(Inst)
#endif

#if VERBOSE >= 6
#define PRINT_6(Inst) if (! disable_debug_msg) {Inst;}
#else
#define PRINT_6(Inst)
#endif

#if VERBOSE >= 7
#define PRINT_7(Inst) if (! disable_debug_msg) {Inst;}
#else
#define PRINT_7(Inst)
#endif

#define ERROR(Msg)    TRACE_DEBUG(Msg << "\n\n"); exit(1);


#endif
