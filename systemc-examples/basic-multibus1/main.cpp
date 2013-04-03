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
         +---------+    +---------------+   +--------+
         |        +++  +-+            +++  +++       |
         | Alice  | +--+ |  RouterAB  | +--+ |  Bob  |
         |        +++  +-+            +++  +++       |
         +---------+    +---------------+   +--------+

         +---------+    +---------------+   +---------+
         |        +++  +-+            +++  +++        |
         | Chloe  | +--+ |  RouterCD  | +--+ |  David |
         |        +++  +-+            +++  +++        |
         +---------+    +---------------+   +---------+
         */

        initiator a("Alice");
        target b("Bob");
        Bus router_ab("RouterAB");
        router_ab.map(b.socket, 0, 100);
        a.socket.bind(router_ab.target);
        router_ab.initiator.bind(b.socket);
    
        initiator c("Chloe");
        target d("David");
        Bus router_cd("RouterCD");
        router_cd.map(d.socket, 0, 100);
        c.socket.bind(router_cd.target);
        router_cd.initiator.bind(d.socket);
    

        /* and start simulation */
        sc_start();
        return 0;
}
