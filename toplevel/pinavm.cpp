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
	cout << "sc_main from pinavm.cpp called." << endl;
	cout << "This function is only usefull at link time, but should never be called" << endl;
	abort();
}

int main(int argc, char **argv) {
	return toplevel_main(argc, argv);
}
