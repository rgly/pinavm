


// This is to include the SystemC constructs and objects.


// We declare the 1-bit ADDER as a "SC_MODULE" with the input and
// output ports declared immediately after the module declarations
// This will add two bits together(a,b), with a carry in(cin) and
// output the sum(sum) and a carry out(cout).
SC_MODULE (BIT_ADDER)
{
       sc_in<bool> x;
 sc_in<bool> y;
 sc_in<bool> cin;

        sc_out<bool> carryout;

 //In the constructor block of the module, declare a method called "process"
        //with a sensitivity list that includes cin.
 SC_CTOR (BIT_ADDER)
 {
  SC_THREAD(process);
  sensitive << cin;
        }

 void ind()
 {
  //Declare variables of type "logic" to be used in calculations
  bool xANDy,xXORy,cinANDxXORy;

  bool xr = x.read();
  bool yr = y.read();
  bool cr = cin.read();

// 		std::cout << "xr : " << xr << "\n";
// 		std::cout << "yr : " << yr << "\n";

  //Perform intermediate calculations for the 1-BIT adder
  //Note to read input from a port, one must use the format: "port_name.read()"
  xANDy = xr & yr;
  xXORy = xr ^ yr;
  cinANDxXORy = cr & xXORy;

  //Calculate sum and carry out of the 1-BIT adder
// 		std::cout << (xXORy ^ cr) << "\n";
  carryout = (xANDy | cinANDxXORy);
 }

       //Define the functionality for the "process" method in the 1-BIT adder
 void process()
        {
  ind();
 }
};




SC_MODULE(INIT)
{
 sc_out<bool> X[8];
 sc_out<bool> Y[8];
 sc_out<bool> Cin;
 sc_in<bool> Cout;
 int i;

 SC_CTOR(INIT)
 {
  SC_THREAD(process);
 }

 void process()
 {
//		std::cout << "INIT launched\n";
 }
};



int sc_main(int argc, char* argv[])
{
 int i;
 INIT vec("vec");

 sc_signal<bool>* x = new sc_signal<bool>();
 sc_signal<bool>* y = new sc_signal<bool>();
 sc_signal<bool>* cin = new sc_signal<bool>();
 sc_signal<bool>* coutSig = new sc_signal<bool>();

 sc_signal<bool>* lastcout = new sc_signal<bool>();

 BIT_ADDER* adder = new BIT_ADDER("FirstAdder");
 BIT_ADDER* previous_adder = adder;

 vec.X[0](*x);
 vec.Y[0](*y);

 adder->x(*x);
 adder->y(*y);
 adder->cin(*cin);
 vec.Cin(*cin);

 for (i = 1; i < 8; i++) {
//		std::cout << "LOOP\n";

  x = new sc_signal<bool>();
  y = new sc_signal<bool>();
  coutSig = new sc_signal<bool>();
  adder = new BIT_ADDER("BitAdder" + i);
//		std::cout << "1\n";

  previous_adder->cout(*coutSig);
  adder->cin(*coutSig);
//		std::cout << "2\n";
  adder->x(*x);
  vec.X[i](*x);
  adder->y(*y);
  vec.Y[i](*y);

  previous_adder = adder;
 }
//	std::cout << "END LOOP\n";

 previous_adder->cout(*lastcout);
 vec.Cout(*lastcout);

//	std::cout << "start\n";

 //
 for (i = 0; i < 8; i++) {
  vec.X[i].write(i & 1);
  vec.Y[i].write(0);
 }
 vec.Cin.write(0);

 //Run the Simulation for "200 nanosecnds"
 sc_start(200,SC_NS);

 return(0);
}
