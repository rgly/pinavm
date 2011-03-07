#include "systemc.h"
#include "Producteur.h"
#include "Consommateur.h"
#include "RendezVous.h"


// FIXME: move this code in a bytecode library
extern "C" void tweto_call_process_method(sc_core::SC_ENTRY_FUNC vfct, sc_core::sc_process_host *host);
void tweto_call_process_method(sc_core::SC_ENTRY_FUNC vfct, sc_core::sc_process_host *host) {
    (host->*vfct)();
}

using namespace std;

int sc_main(int, char**)
{
    
    Consommateur             consommateur("Consommateur");
    Producteur               producteur("Producteur");
    RendezVous<int>          rendezvous("RendezVous");
    
    //Consommateur             consommateur2("Consommateur2");
    
    producteur.sortie(rendezvous);
    consommateur.entree(rendezvous);
    
    //consommateur2.entree(rendezvous);
    
   sc_start();

   return 0;
}

