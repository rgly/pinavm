/*
Test a WHILE with one NOTIFY instruction
*/

#include "systemc.h"

using namespace sc_core;
using namespace std;

SC_MODULE(thread){
  sc_event e1;

  void T1(){
    int a=0;
    while(a<4){
      e1.notify();
      a++;
    }
  }


  SC_HAS_PROCESS(thread);
  thread(sc_module_name name){
    SC_THREAD(T1);
    sensitive << e1; /* penser a ajouter tous les
    //        evenements mentionnes dans T1 */
  }
};


int sc_main(int argc, char *argv[]){
  thread MyFirstThread("toto");
  sc_start();
  return 0;
}
