#include <iostream>
#include "systemc.h"

SC_MODULE(TestBench) {
	
	sc_out<sc_uint<8> > a, b;
	sc_in<sc_uint<8> > c;
	
	SC_CTOR(TestBench);
	
	void genIn();
	
};



