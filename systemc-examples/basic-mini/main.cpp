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
#include <sysc/pinavm/permalloc.h>


using namespace std;
using namespace sc_core;

struct initiator : sc_module {
        basic::initiator_socket<initiator> socket;
        void thread(void) {
            
            for (basic::addr_t addr = 4; addr <= 30; addr+=4) {
                socket.write(addr, 42);
            }
            
        }
        SC_CTOR(initiator) {
                SC_THREAD(thread);
        }
};

struct target : sc_module, basic::target_module_base {
        basic::target_socket<target> socket;
        tlm::tlm_response_status write(basic::addr_t a,
                                       const basic::data_t &d) {
                cout << "j'ai reçu : " << d << " à l'adresse " << a << endl;
                return tlm::TLM_OK_RESPONSE;
        }
        tlm::tlm_response_status read(basic::addr_t a,
                                       /* */ basic::data_t &d) {
                SC_REPORT_ERROR("TLM", "non implémenté");
                abort();
        }
        SC_CTOR(target) {/* */}
};

int sc_main (int argc, char ** argv) {
        /*
         +---------+    +-------------+    +--------+
         |        +++  +-+           +++  +++       |
         | Alice  | +--+ |  Router   | +--+ |  Bob  |
         |        +++  +-+           +++  +++       |
         +---------+    +-------------+    +--------+
         */

        initiator* a = permalloc::obj<initiator>("Alice");
        target* b = permalloc::obj<target>("Bob");
        Bus* router = permalloc::obj<Bus>("Router");

        /* Bob is mapped at addresses [0, 100[, i.e. ... */
        router->map(b->socket, 4, 100);

        /* connect components to the bus */
        a->socket.bind(router->target);
        router->initiator.bind(b->socket);

        /* and start simulation */
        sc_start();
        return 0;
}
