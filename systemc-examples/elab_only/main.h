#include <systemc.h>

SC_MODULE(Component)
{
	sc_out < bool > out;
	sc_in < bool > in;
		
	SC_CTOR(Component) {
	}	
};
