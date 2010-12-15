#ifndef TARGET_H
#define TARGET_H

#include <systemc>
using namespace sc_core;

struct sc_verif
{
	static void ASSERT(bool cond) {
		//abort();
		;
	}
};

class Target : public sc_module {
public:
	sc_in<bool> in;
	void compute();
	SC_CTOR(Target);
};

#endif // TARGET_H
