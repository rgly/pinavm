/*
  This is not a real SystemC program, but just a C++ file, with an
  sc_main function and a call to pinavm_callback(), to test the JIT
  machinery to call sc_main and get back "pinavm_callback" on a
  simple example.

  $ cd systemc-examples/plain-cpp/
  $ cd toplevel
  $ make pinavm
  $ ./pinavm ../systemc-examples/plain-cpp/test.bc

  Should do it.
*/
#include <stdio.h>
#include <iostream>

extern "C" int sc_main(int argc, char **argv);

extern "C"
void pinavm_callback();

int sc_main(int argc, char **argv)
{
	std::cout << "sc_main() called\n";
	std::cout << std::endl;
	pinavm_callback();
	return 0;
}

extern "C" int main (int argc, char **argv) {
	std::cout << "main() called\n";
	std::cout << std::endl;
	return 0;
}
