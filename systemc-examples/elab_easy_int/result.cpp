





SC_MODULE(COMPONENT)
{
 sc_out<bool> out;
 sc_in<bool> in;
 bool notified;
 bool isHead;
 void process() {
  if (! isHead) {
   wait(in.default_event());
  }
  notified = true;
  out.write(true);
 }



 SC_CTOR(COMPONENT) {
  notified = false;
  isHead = false;
  SC_THREAD(process);
 }
};

int sc_main (int argc , char *argv[])
{
 sc_signal<bool> s1, s2, s3;
 COMPONENT C1("C1"), C2("C2"), C3("C3");

 C1.out(s1);
 C2.out(s2);
 C3.out(s3);

 C1.in(s3);
 C2.in(s1);
 C3.in(s2);

 C1.isHead = true;

 //Run the Simulation for "200 nanosecnds"
 sc_start(200, SC_NS);

 return 0;
}

SC_MODULE(COMPONENT)
{
 sc_out<bool> out;
 sc_in<bool> in;
 bool notified;
 bool isHead;
 void process() {
  if (! isHead) {
   wait(in.default_event());
  }
  notified = true;
  out.write(true);
 }



 SC_CTOR(COMPONENT) {
  notified = false;
  isHead = false;
  SC_THREAD(process);
 }
};

int sc_main (int argc , char *argv[])
{
 sc_signal<bool> s1, s2, s3;
 COMPONENT C1("C1"), C2("C2"), C3("C3");

 C1.out(s1);
 C2.out(s2);
 C3.out(s3);

 C1.in(s3);
 C2.in(s1);
 C3.in(s2);

 C1.isHead = true;

 //Run the Simulation for "200 nanosecnds"
 sc_start(200, SC_NS);

 return 0;
}
