all: bus.o initiator_socket.o target_socket.o

%.o: %.cpp ${wildcard *.h}
	g++ -c -I../TLM-2009-07-15/include/tlm/ -I../systemc-2.2.0/src/ $< -o $@
