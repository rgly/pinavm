#ifndef _CONFIG_H
#define _CONFIG_H

#include "llvm/Support/raw_ostream.h"

extern bool disable_debug_msg;

#define VERBOSE 7

#define TRACE(Msg) llvm::outs() << Msg; llvm::outs().flush();

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

/*!
 * Essentially like abort(), but not sensitive to NDEBUG being defined
 */
#define ASSERT(expr)				\
	do {					\
		if (!(expr)) {			\
			TRACE(__FILE__ << ":" << __LINE__ << ": assertion failed: \"" #expr "\"\n");	\
			abort();		\
		};				\
	} while (0)

/*!
 * Use with either ABORT("blablabla") or ABORT("blabla" << variable <<
 * "blabla")
 */
#define ABORT(reason)				\
	do {					\
		TRACE(__FILE__ << ":" << __LINE__ << ": execution aborted:\n" << reason << "\n"); \
		abort();			\
	} while (0)

#endif
