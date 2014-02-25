/* This file defines weak symbols to enable the embedded SystemC library to be used
   without PinaVM. Define NO_WEAK_SYMBOLS to disable them, in which case this SystemC
   library will need either PinaVM or a strong-symbol version of this file to work. */

#ifndef NO_WEAK_SYMBOLS
#define WEAK __attribute__((weak))

#include "systemc"

extern "C" WEAK void pinavm_callback (sc_core::sc_simcontext* sc, const sc_core::sc_time& d)
{
	sc->simulate (d);
}

extern "C" WEAK void tweto_optimize_process (void)
{
}

void WEAK tweto_mark_const (void const* x, unsigned long y)
{
}

/*extern "C"*/ int launch_systemc (int, char**);

extern "C" WEAK int main (int argc, char* argv[])
{
	launch_systemc (argc, argv);
}

bool disable_debug_msg;

#endif

