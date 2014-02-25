#include "Consommateur.h"

Consommateur::Consommateur(sc_module_name name) : sc_module(name)
{
	cout << "le consommateur est activé" << endl;
	cout << "Port d'adresse " << entree.get_interface() << endl;
   SC_THREAD(consommation);
}

void Consommateur::consommation()
{
   while (true)
   {

	cout << "Veut recevoir" << endl;
	   
      int valeur_recue = entree->get(); 

      cout << "Recu : " << valeur_recue << endl;
   }
}
