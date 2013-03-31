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
        
        // Send to Bob
        basic::addr_t addr1 = 4;
        cout << "Writing to Bob at address " << addr1 << endl;
        for (basic::data_t val = 1; val <= 10; val++) {
            val++;
            socket.write(addr1, val);
        }
        // Send to Leo
        basic::addr_t addr2 = 60;
        cout << "Writing to Leo at address " << addr2 << endl;
        for (basic::data_t val = 20; val <= 30; val++) {
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
     | Alice  | +--+ |  Router   | +----+ |  Bob  |
     |        +++  +-+           | +--  +++       |
     +---------+    +            +++ |   +--------+
                    +-------------+  |
                                     |   +--------+
                                     |  +++       |
                                     ---+ |  Leo  |
                                        +++       |
                                         +--------+
     */     
    
    
    initiator a("Alice"); 
    target b1("Bob");
    target b2("Leo");
    Bus router("Router");
    
    /* Bob is mapped at addresses [0, 49[, i.e. ... */
    router.map(b1.socket, 0, 49);
    /* Leo is mapped at addresses [50, 100[, i.e. ... */
    router.map(b2.socket, 50, 100);
    
    /* connect components to the bus */
    a.socket.bind(router.target);
    router.initiator.bind(b1.socket);
    router.initiator.bind(b2.socket);
    
    /* and start simulation */
    sc_start();
    return 0;
}
