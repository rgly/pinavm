// Spin/SMV SystemC Verification Benchmark
// by Jerome Cornet

#include <sstream>
#include "systemc.h"

using namespace std;

// class my_if : public sc_interface
// {
// public:
//    virtual void f()=0;
// };

SC_MODULE(MyModule)
{
	MyModule* initiator;
	sc_event e;
	
	SC_HAS_PROCESS(MyModule);
	MyModule(sc_module_name name)
	{
		SC_THREAD(compute);
		sensitive << e;
	}
	
	void fonct()
	{
		e.notify();
	}
	
	void compute()
	{
		wait(e);      
		initiator->fonct();
	}
};

SC_MODULE(Source)
{
	MyModule* initiator;
	
	
	SC_HAS_PROCESS(Source);
	Source(sc_module_name name, int number_)
	{
		SC_THREAD(compute);
	}
	
	void compute()
	{
		wait(1);
		initiator->fonct();
	}
};



int sc_main(int argc, char **argv)
{
   int nb_modules;
   
   cout << "Start : \n";

   if (argc >= 2)
   {
	   stringstream ss;
	   
	   ss << argv[1];
	   ss >> nb_modules;
	   
	   if (nb_modules < 3)
	   {
		   cerr << "Number of modules should be at least 3." << endl;
		   return 1;
	   } else {
		   cout << "Number of modules : " << nb_modules << "\n";

	   }
   }
   else
   {
	   cerr << "usage: run.x number_of_modules" << endl;
	   return 1;
   }
   
    Source source("Source", 0);
    MyModule **myModule = new MyModule*[nb_modules-2];
   
    for (int i=0; i<nb_modules-2; i++)
    {
	    stringstream ss;
	    ss << "MyModule" << i+1;
	    myModule[i] = new MyModule(ss.str().c_str());
    }
    MyModule sink("Sink");
    sink.initiator = &sink;
    source.initiator = myModule[0];
    
    for (int i=0; i<nb_modules-2; i++)
    {
	    if (i != (nb_modules-3))
	    {
	      myModule[i]->initiator = myModule[i+1];
	    }
	    else
	    {
		    myModule[i]->initiator = &sink;
	    }
    }
    
    sc_start();
    
//     for (int i=0; i<nb_modules-2; i++)
//     {
// 	    delete myModule[i];
//     }
//     delete [] myModule;
    
    return 0;
}
