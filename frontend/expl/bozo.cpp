#include <systemc.h>

//static bool myglobalvar = false;

//extern void wait();
//extern void notify();
//extern void wait(int t);

class mytop : public sc_module
{
  public:
  bool x;
  int res;
  sc_event e;
  SC_HAS_PROCESS(mytop);

  mytop(sc_module_name name) :
    sc_module(name) {
    SC_THREAD(myFctP);
    SC_THREAD(myFctQ);
  }
  
  void myFctP() {
    //cout <<"mfuncp" << endl;
    wait(e);
    
    //    cout <<"p" << endl;
    wait(10,SC_NS);
    if (x)
      res = 0;
    //      cout << "mfunc Ok" << endl;
    else
      //      cout << "mfunc Ko" << endl;
      res = 1;
    //    cout <<"mfunc end p" << endl;
  }
  
  void myFctQ() {
    int moniterateur;
    //    cout <<"mfunc q" << endl;
    e.notify();
    x = false;
    wait(10,SC_NS);
    x = true;
    //    cout <<"mfunc end q" << endl;

    for (moniterateur = 0;
	 moniterateur < 10;
	 moniterateur++) {
      x = !x;
    }
  }
};
 

int sc_main( int argc, char* argv[] )
{
  int i, j;
  mytop MYTOP("MYTOP");
  mytop OTHERTOP("OTHERTOP");

  ::std::cout << "Entering sc_main...\n";
//   MYTOP.myFctP();
//   MYTOP.myFctQ();
  //    i = 5;
  sc_start();
  //    j = handleI(i);
}


// int handleI(int i)
// {
//   char* mymsg = "handling i";
//   int myres = i + 2;

//   return myres;
// }

// void execute()
// {
//   int i, j;
// //   top TOP("TOP");
// //   sc_start(-1);
//   i = 5;
  
//   j = handleI(i);
// }
