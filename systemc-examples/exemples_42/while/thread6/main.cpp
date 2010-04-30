/*
Test a WHILE with one WAIT before and one WAIT after

!!!!!!!!!!!!!! BUG !!!!!!!!!!!!!!!

Ne traite pas le dernier WAIT !
*/

#include "systemc.h"

using namespace sc_core;
using namespace std;

SC_MODULE(thread){
  sc_event e1,e2;

  void T1(){
    int a=0;
    wait(e1);
    while(a<4){
      wait(e2);
      a++;
    }
    wait(e2);
  }


  SC_HAS_PROCESS(thread);
  thread(sc_module_name name){
    SC_THREAD(T1);
    sensitive << e1 << e2; /* penser a ajouter tous les
    //        evenements mentionnes dans T1 */
  }
};


int sc_main(int argc, char *argv[]){
  thread MyFirstThread("toto");
  sc_start();
  return 0;
}
