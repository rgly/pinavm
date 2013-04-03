/*
 *  This file describe some useful function which is used by tweto
 *  backend. the test case for tweto backend should include this file.
 */


#include "basic.h"
#include "bus.h"
// FIXME: move this code in a bytecode library
extern "C" void tweto_call_process_method(
	sc_core::SC_ENTRY_FUNC vfct, sc_core::sc_process_host *host)
{
    (host->*vfct)();
};
extern "C" tlm::tlm_response_status tweto_call_write_method(
	basic::target_module_base *module, basic::addr_t a, basic::data_t d)
{
    return module->write(a, d);
};
extern "C" tlm::tlm_response_status tweto_call_read_method(
	basic::target_module_base *module, basic::addr_t a, basic::data_t d)
{
    return module->read(a, d);
};
