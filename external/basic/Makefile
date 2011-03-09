all: bus.o

%.o: %.cpp ${wildcard *.h}
	g++ -c -I../TLM-2009-07-15/include/tlm/ -I../systemc-2.2.0/src/ bus.cpp -o bus.o
