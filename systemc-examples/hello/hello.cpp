#include <sstream>
#include "systemc.h"

using namespace std;

struct MyModule: sc_core::sc_module {
  const uint32_t in;
  uint32_t out;
  SC_HAS_PROCESS(MyModule);
  MyModule(sc_core::sc_module_name name, uint32_t data):
    sc_module(name), in(data), out(0)
  {
    SC_METHOD(compute);
  }

  void compute() {
    out += in + 30;
    cerr <<name() <<".compute: out = " <<out <<"\n";
  }
};

int sc_main(int argc, char *argv[]) {
  cerr <<"sc_main: Hello World!\n";
  MyModule my_module("MY_MODULE",12);
  sc_core::sc_start();
  cerr <<"sc_main: Bye bye!\n";
  return my_module.out-42;
}
