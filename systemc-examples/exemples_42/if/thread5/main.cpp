/*
Test the presence of a IF..THEN..ELSE
in a IF branch at the beginning of the process
*/

#include "systemc.h"

using namespace sc_core;
using namespace std;

SC_MODULE(thread){
  sc_event e1,e2,e3;

  void T1(){
    int a=0;
    if(a==0){
      wait(e1);
      a++;
      if(a==0){
	e2.notify();
	a++;
      }
      else{
	wait(e2);
	a++;
      }
    }
    else{
      e3.notify();
      a++;
    }
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
