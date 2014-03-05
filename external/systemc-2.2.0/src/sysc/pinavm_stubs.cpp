/* This file defines weak symbols to enable the embedded SystemC library to be used
   without PinaVM. Define NO_WEAK_SYMBOLS to disable them, in which case this SystemC
   library will need either PinaVM or a strong-symbol version of this file to work. */

#ifndef NO_WEAK_SYMBOLS
#define WEAK __attribute__((weak))

#include "systemc"
#include <assert.h>

extern "C" WEAK void pinavm_callback (sc_core::sc_simcontext* sc, const sc_core::sc_time& d)
{
	sc->simulate (d);
}

void WEAK tweto_mark_const (void const* x, unsigned long y)
{
}

// C++ linkage function
int launch_systemc (int, char**);

extern "C" WEAK int main (int argc, char* argv[])
{
	launch_systemc (argc, argv);
}

// necessary to run lli with LD_PRELOAD of the internal SystemC lib
extern "C" WEAK int sc_main (int, char**)
{
	assert (0);
}

bool WEAK disable_debug_msg;

#endif

