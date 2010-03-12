/*
  This is not a real SystemC program, but just a C++ file, with an
  sc_main function and a call to pinapa_callback(), to test the JIT
  machinery to call sc_main and get back "pinapa_callback" on a
  simple example.

  $ cd systemc-examples/plain-cpp/
  $ cd toplevel
  $ make pinapa
  $ ./pinapa ../systemc-examples/plain-cpp/test.bc

  Should do it.
*/
#include <stdio.h>
#include <iostream>

extern "C" int sc_main(int argc, char **argv);

extern "C"
void pinapa_callback();

int sc_main(int argc, char **argv)
{
	std::cout << "sc_main() called\n";
	std::cout << std::endl;
	pinapa_callback();
	return 0;
}

extern "C" int main (int argc, char **argv) {
	std::cout << "main() called\n";
	std::cout << std::endl;
	return 0;
}
