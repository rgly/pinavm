/*
Test the presence of a IF in a WHILE branch
*/

#include "systemc.h"

using namespace sc_core;
using namespace std;

SC_MODULE(thread){
  sc_event e1,e2;

  void T1(){
    int a=0;
    while(a<4){
      e1.notify();
      if(a<8){
	wait(e2);
      }
      wait(e1);
      a++;
    }
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
