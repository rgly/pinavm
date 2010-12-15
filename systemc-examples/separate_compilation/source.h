#ifndef SOURCE_H
#define SOURCE_H

#include <systemc.h>
using namespace sc_core;

class Source : public sc_module {
public:
	void compute();
	sc_out<bool> out;
	SC_CTOR(Source);
};

#endif // SOURCE_H
