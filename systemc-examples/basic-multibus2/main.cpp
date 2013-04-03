/********************************************************************
 * Copyright (C) 2009 by Ensimag and Verimag                        *
 * Initial author: Matthieu Moy                                     *
 ********************************************************************/

/* Plateforme SystemC utilisant le protocole TLM BASIC.

   Cette version est en un seul fichier pour faciliter la
   compréhension. Une version multi-fichiers (donc plus propre) est
   également disponible.
*/

#include "basic.h"
#include "bus.h"


using namespace std;
using namespace sc_core;

struct initiator : sc_module {
        basic::initiator_socket<initiator> socket;
        void thread(void) {
            
        cout << name() << " envoi des données..." << endl;
		basic::addr_t addr = 4;
        for (basic::data_t val = 1; val <= 10; val++) {
            val++; 
            socket.write(addr, val);
        }
        cout << name() << " envoi des données..." << endl;
        basic::addr_t addr2 = 64;
        for (basic::data_t val = 1; val <= 10; val++) {
            val++; 
            socket.write(addr2, val);
        }
    
        }
        SC_CTOR(initiator) {
                SC_THREAD(thread);
        }
};

struct target : sc_module, basic::target_module_base {
        basic::target_socket<target> socket;
        tlm::tlm_response_status write(const basic::addr_t &a,
                                       const basic::data_t &d) {
                cout << name() << " à reçu : " << d << endl;
                return tlm::TLM_OK_RESPONSE;
        }
        tlm::tlm_response_status read (const basic::addr_t &a,
                                       /* */ basic::data_t &d) {
                SC_REPORT_ERROR("TLM", "non implémenté");
                abort();
        }
        SC_CTOR(target) {/* */}
};

int sc_main (int argc, char ** argv) {
    /*
     +---------+    +-------------+      +--------+
     |        +++  +-+           +++    +++       |
     | Alice  | +--+ | RouterABC | +----+ |  Bob  |
     |        +++  +-+           | +--  +++       |
     +---------+    +            +++ |   +--------+
                    +-------------+  |
                                     |   +---------+
                                     |  +++        |
                                     ---+ |  Chloe |
                                        +++        |
                                         +---------+
     
     +---------+    +-------------+      +--------+
     |        +++  +-+           +++    +++       |
     | David  | +--+ | RouterDEF | +----+ |  Ed  |
     |        +++  +-+           | +--  +++       |
     +---------+    +            +++ |   +--------+
                    +-------------+  |
                                     |   +---------+
                                     |  +++        |
                                     ---+ |  Foo   |
                                        +++        |
                                         +---------+
     
     */ 

        initiator a("Alice");
        target b("Bob");
        target c("Chloe");
        Bus router_abc("RouterABC");
        router_abc.map(b.socket, 0, 50);
        router_abc.map(c.socket, 51, 100);
        a.socket.bind(router_abc.target);
        router_abc.initiator.bind(b.socket);
        router_abc.initiator.bind(c.socket);
    
        initiator d("David");
        target e("Ed");
        target f("Foo");
        Bus router_def("RouterDEF");
        router_def.map(e.socket, 0, 50);
        router_def.map(f.socket, 51, 100);
        d.socket.bind(router_def.target);
        router_def.initiator.bind(e.socket);
        router_def.initiator.bind(f.socket);
    

        /* and start simulation */
        sc_start();
        return 0;
}
