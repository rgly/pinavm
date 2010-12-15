all: promela

SRC=main.cpp
# This platform artificially requires argv[1] to be 42
ARG=42

include ../common.mk

# PinaVM will load this library, then bitcode function calls to this
# library will be resolved properly.
PINAVM_LIBS=./libdummy.so

QUIET_MODE=no

main.pr: libdummy.so

libdummy.so: dummylib.cpp
	g++ -shared $< -o $@
