#ifndef _TWETOBACKEND_H
#define _TWETOBACKEND_H


struct Frontend;
extern void launch_twetobackend(Frontend *fe, ExecutionEngine *ee);



#include <cstdlib>

namespace sc_core {
    
    class sc_process_host;
    
    typedef void (sc_process_host::*SC_ENTRY_FUNC)();
    typedef void (*SC_ENTRY_FUNC_OPT)();
    
} // namespace sc_core

 
extern "C" sc_core::SC_ENTRY_FUNC_OPT
tweto_optimize_process(sc_core::SC_ENTRY_FUNC fct, sc_core::sc_process_host *arg);

extern void tweto_mark_const(const void *ptr_to_cst, size_t size);


#endif
