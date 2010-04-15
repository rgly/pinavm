/*
Test with a WAIT at the first position.
We must have a transition "{}op/{}" between the STATE_0
and the STATE_1, which corresponds to the start of the process.
*/

#include "systemc.h"

using namespace sc_core;
using namespace std;

SC_MODULE(thread){
  sc_event e1,e2,e3;

  void T1(){
    int a=0;
    a++;
    wait(e1);
    e2.notify();
    e3.notify();
  }


  SC_HAS_PROCESS(thread);
  thread(sc_module_name name){
    SC_THREAD(T1);
    sensitive << e1 << e2 << e3; /* penser a ajouter tous les
    //        evenements mentionnes dans T1 */
  }
};


int sc_main(int argc, char *argv[]){
  thread MyFirstThread("toto");
  sc_start();
  return 0;
}
