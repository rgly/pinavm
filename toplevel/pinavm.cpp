/********************************************************************
 * Copyright (C) 2010 by Verimag                                    *
 * Initial author: Matthieu Moy                                     *
 * This file must not be distributed outside Verimag                *
 ********************************************************************/

/*!
  \file main.cpp
  \brief Program for the main PinaVM executable.

  
*/

#include "toplevel.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

// Defined this way somewhere in systemc.h.
extern "C" int sc_main(int argc, char **argv);

int sc_main(int argc, char **argv) {
	std::cout << "sc_main from pinavm.cpp called.\n" ;
	std::cout << "This function is only usefull at link time, but should never be called\n"  ;
	abort();
}

int main(int argc, char **argv) {
	// Move exception handle here,
	// since toplevel.cpp is compiled without rtti & exception.
	try {
		toplevel_main(argc, argv);
	} catch(const std::string & msg) {
		std::cerr << argv[0] << ": " << msg << "\n";
	} catch(...) {
		std::cerr << argv[0] <<
		    ": Unexpected unknown exception occurred.\n";
	}
	return 0;
}
