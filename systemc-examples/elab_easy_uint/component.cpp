#include <systemc.h>
#include "component.h"

void Component::main() {
	out->write(true);
	if (! isHead) {
		while (! in.read())
//				sc_core::wait(5, SC_NS);
			wait(5, SC_NS);
	}
	notified = true;
}
