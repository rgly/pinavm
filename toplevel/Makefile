SRCS = ${wildcard *.cpp}
TARGET = libpinapa.so

include ../Makefile.common

ALL_OBJS = $(OBJS) ${wildcard $(SYSTEMCFRONTEND)/*.o} ${wildcard $(LLVMBACKENDS)/Debug/*.o} $(SYSTEMCROOT)/lib-linux/libsystemc.a

CPPFLAGS += -I$(SYSTEMC)/include
CPPFLAGS += -I$(SYSTEMCFRONTEND)
CPPFLAGS += -I$(LLVMBACKENDSHEADERS)
CPPFLAGS += `llvm-config --cxxflags jit bitreader bitwriter native`

LDLIBS += `llvm-config --ldflags --libs jit bitreader bitwriter native` 

$(TARGET): $(OBJS) 
	cd $(SYSTEMCFRONTEND) && make all
	cd $(LLVMBACKENDS) && make
	g++ -shared -o $@ $(ALL_OBJS) $(LDFLAGS) $(LDLIBS)
