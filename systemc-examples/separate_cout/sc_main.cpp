#include <systemc>
using namespace sc_core;

#include "source.h"
#include "target.h"

int sc_main (int argc , char *argv[]) 
{
 	sc_signal<bool> s1;

 	Source source("source");
 	Target target("target");
       
	source.out(s1); target.in(s1);

	//Run the Simulation for "200 nanosecnds"
	sc_start(200, SC_NS);

	return 0;
}
