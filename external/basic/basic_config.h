#ifndef BASIC_CONFIG_H
#define BASIC_CONFIG_H

// The following macros is from pinavm/frontend/config.h,
// because config.h includes raw_ostream of llvm, and we also 
// need raw_ostream to print llvm::Type on screen.
// This file will be included by user's systemc code, I do not
// think mixing systemc code with llvm headers is a good idea.
extern bool disable_debug_msg;
#include <iostream>
#define BASIC_TRACE(Msg) std::cout << Msg; std::cout.flush();
#define BASIC_TRACE_DEBUG(Msg) if (! disable_debug_msg) { BASIC_TRACE(Msg) }



#endif
