/*
Test 3 imbricate IF..THEN..ELSE
in a IF branch
*/

#include "systemc.h"

using namespace sc_core;
using namespace std;

SC_MODULE(thread){
  sc_event e1,e2,e3,e4;

  void T1(){
    int a=0;
    e1.notify();
    if(a==0){
      wait(e1);
      a++;
      if(a>0){
	e3.notify();
	wait(e2);
	e3.notify();
	if(a==4){
	  wait(e4);
	  wait(e4);
	  e4.notify();
	}
	else{
	  wait(e1);
	}
	wait(e3);
      }
      else{
	e1.notify();
	wait(e1);
      }
    }
    else{
      e2.notify();
    }
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
