SC_MODULE(DEMULTIPLEXER)
{
 sc_core::sc_out<int> output[4];
//	sc_core::sc_out<int> ticks[NB_COMPONENTS];

 SC_CTOR(DEMULTIPLEXER) {
  SC_THREAD(process);
 }

 void process()
 {
  int i;
  for (i = 0 ; i < 4 ; i++) {
   output[i].write((i+1) * 2);
  }
 }
};


SC_MODULE(COMPONENT)
{
 sc_core::sc_in<int> data;
 int dr;

 SC_CTOR(COMPONENT) {
  SC_THREAD(process);
  sensitive << data;
  this->dr = 0;
 }

 void process() {
  sc_core::wait(data->default_event());
  dr = data.read();
  std::cout << "\n";
  std::cout << dr << "\n";
 }
};

int sc_main (int argc , char *argv[])
{
 DEMULTIPLEXER dm("Demultiplexer");
 int i;
 sc_core::sc_signal<int> s1, s2, s3, s4;
 COMPONENT C1("C1"), C2("C2"), C3("C3"), C4("C4");

 dm.output[0](s1);
 dm.output[1](s2);
 dm.output[2](s3);
 dm.output[3](s4);

 C1.data(s1);
 C2.data(s2);
 C3.data(s3);
 C4.data(s4);

 //Run the Simulation for "200 nanosecnds"
 sc_start(200, SC_NS);

 return 0;
}
