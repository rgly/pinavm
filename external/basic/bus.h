#ifndef BUS_H
#define BUS_H

#ifndef BASIC_H
#error Please include file "basic.h"
#endif

#include <map>

SC_MODULE(Bus), basic::target_module_base, 
	/* Allow PinaVM to store buses in channelsMap */ 
	virtual public sc_core::sc_interface {

	basic::initiator_socket<Bus, true> initiator;
	basic::target_socket<Bus, true> target;

	Bus(sc_core::sc_module_name name);

	tlm::tlm_response_status
		read(const basic::addr_t &a, /* */ basic::data_t& d);

	tlm::tlm_response_status
		write(const basic::addr_t &a, const basic::data_t &d);

	void map(basic::compatible_socket& port, basic::addr_t start_addr, basic::addr_t size);
    bool checkAdressRange(const basic::addr_t &a);
    bool checkAdressConcordance(basic::compatible_socket *target, 
                                const basic::addr_t &a);
        
private:

	void print_addr_map();
	void end_of_elaboration();

	class addr_range {
	public:
		addr_range(basic::addr_t b, basic::addr_t e) :
			begin(b), end(e)
			{}
		const basic::addr_t begin;
		const basic::addr_t end;
		bool operator<(const addr_range& ar) const {
			return(end < ar.begin);
		}
	};

	typedef std::multimap<basic::compatible_socket*, addr_range> port_map_t;
	port_map_t port_map;

	typedef std::map<addr_range, int> addr_map_t;
	addr_map_t addr_map;
};

#endif
