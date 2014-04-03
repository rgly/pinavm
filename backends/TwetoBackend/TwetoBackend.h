#ifndef _TWETOBACKEND_H
#define _TWETOBACKEND_H

#include "BackendOption.h"
namespace sc_core {
    
    class sc_time;
    class sc_simcontext;
    
} 

enum tweto_opt_level {
	noopt,
	dynopt,
	staticopt,
};

class Frontend;
extern void launch_twetobackend(Frontend * fe, 
                                sc_core::sc_simcontext* simcontext, 
                                const sc_core::sc_time& simduration, 
                                enum tweto_opt_level optimize, bool disableMsg);

#endif

