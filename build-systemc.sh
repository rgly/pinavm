#! /bin/sh

if [ ! -f external/systemc-2.2.0/objdir-gcc/Makefile ]; then 
    cd external/systemc-2.2.0/ && ./autogen.sh && 
    mkdir -p objdir-gcc/ && cd objdir-gcc/ && 
    ../configure &&
    cd ../../../
fi
cd external/systemc-2.2.0/objdir-gcc/ && $MAKE pthreads_debug
