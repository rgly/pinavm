#ifndef _TWETOBACKEND_H
#define _TWETOBACKEND_H

#include "BackendOption.h"
namespace sc_core {
    
    class sc_process_host;
    class sc_time;
    class sc_simcontext;
    
    typedef void (sc_process_host::*SC_ENTRY_FUNC)();
    typedef void (*SC_ENTRY_FUNC_OPT)();

} 

enum tweto_opt_level {
	noopt,
	dynopt,
	staticopt,
};

struct Frontend;
extern void launch_twetobackend(Frontend * fe, 
                                sc_core::sc_simcontext* simcontext, 
                                const sc_core::sc_time& simduration, 
                                enum tweto_opt_level optimize, bool disableMsg);

extern "C" sc_core::SC_ENTRY_FUNC_OPT
tweto_optimize_process(sc_core::SC_ENTRY_FUNC fct, sc_core::sc_process_host *arg);

extern void tweto_mark_const(const void *ptr_to_cst, size_t size);

#endif

