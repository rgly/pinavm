#include "systemc.h"
#include "Producteur.h"
#include "Consommateur.h"
#include "RendezVous.h"

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

