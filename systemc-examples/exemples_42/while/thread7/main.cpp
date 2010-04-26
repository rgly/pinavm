/*
Test three imbricate WHILE
*/

#include "systemc.h"

using namespace sc_core;
using namespace std;

SC_MODULE(thread){
  sc_event e1,e2,e3,e4;

  void T1(){
    int a=0;
    wait(e1);
    while(a<4){
      wait(e2);
      while(a<8){
	e3.notify();
	wait(e2);
	while(a<12){
	  wait(e3);
	  a++;
	  wait(e4);
	}
	a++;
      }
      a++;
      e2.notify();
    }
    wait(e3);
  }


  SC_HAS_PROCESS(thread);
  thread(sc_module_name name){
    SC_THREAD(T1);
    sensitive << e1 << e2 << e3 << e4; /* penser a ajouter tous les
    //        evenements mentionnes dans T1 */
  }
};


int sc_main(int argc, char *argv[]){
  thread MyFirstThread("toto");
  sc_start();
  return 0;
}
