#include "systemc.h"
#include "Adder.h"
#include "TestBench.h"

using namespace std;


int sc_main(int, char **) {

	Adder add("Adder");
	TestBench test("TestBench");
	sc_signal<sc_uint<8> > in1, in2;
	sc_signal<sc_uint<8> > out;

	test.a(in1);
	test.b(in2);
	test.c(out);
	
	add.a(in1);
	add.b(in2);
	add.c(out);
	
	
	sc_trace_file *trace;
	
	trace = sc_create_vcd_trace_file("trace");
	
	sc_trace(trace, in1, "in1");
	sc_trace(trace, in2, "in2");
	sc_trace(trace, out, "out");
	
	sc_start(500, SC_NS);
	
	sc_close_vcd_trace_file(trace);
	
	
	return 0;
}
/*
 
 sc_trace_file *trace;
 
 trace = sc_create_vcd_trace_file("trace");
 
 sc_trace(trace, sclk, "clk");
 sc_trace(trace, sreset, "reset");
 sc_trace(trace, scount, "count");
 
 sc_start(500, SC_NS);
 
 sc_close_vcd_trace_file(trace);
*/