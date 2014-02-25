#include "Producteur.h"

Producteur::Producteur(sc_module_name name) : sc_module(name)
{
	cout << "Port d'adresse " << sortie.get_interface() << endl;
   SC_THREAD(production);
}

void Producteur::production()
{
   for (int i=0; i<10; i++) 
   {
      cout << "Envoi de " << i << endl;
	   
      sortie->put(i); 

      cout << "A envoyé" << endl;
   } 
}
