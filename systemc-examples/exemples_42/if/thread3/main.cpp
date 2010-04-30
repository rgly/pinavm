/*
Test two consequential IF..THEN..ELSE
at the beginning of the process
*/

#include "systemc.h"

using namespace sc_core;
using namespace std;

SC_MODULE(thread){
  sc_event e1,e2,e3,e4;

  void T1(){
    int a=0;
    int b=0;
    if(a==0){
      wait(e1);
    }
    else{
      wait(e2);
    }
    if(b==0){
      wait(e3);
    }
    else{
      wait(e4);
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
