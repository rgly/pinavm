#ifndef _HELLOBACKEND_CPP
#define _HELLOBACKEND_CPP

// Implement a interface for toplevel to call, PinaVM will directly
// call launch_xxxxbackend(Frontend*,BackendOption&) if frontend is finished.
#include "BackendOption.h"
class Frontend;
extern void echo_helloworld();
void launch_hellobackend(Frontend* fe, BackendOption& option) {
    echo_helloworld();
}

#endif
