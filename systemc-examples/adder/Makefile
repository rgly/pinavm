SYSTEMCROOT = /Library/SystemC/systemc-2.2.0
ARCH = macosx

CC = llvm-g++
CFLAGS = -Wall -emit-llvm
EXEC_NAME = main
#-emit-llvm
INCLUDES = -I. -I$(SYSTEMCROOT)/include
LIBS = -L. -L$(SYSTEMCROOT)/lib-$(ARCH) -lsystemc -lm
OBJ_FILES = main.o Adder.o TestBench.o

all : $(EXEC_NAME)

clean :
	rm $(EXEC_NAME) $(OBJ_FILES) trace.vcd

$(EXEC_NAME) : $(OBJ_FILES)
	$(CC) -o $(EXEC_NAME) $(OBJ_FILES) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

%.o: %.cc
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

%.o: %.c
	gcc $(CFLAGS) $(INCLUDES) -o $@ -c $<