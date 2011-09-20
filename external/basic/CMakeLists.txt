FILE(GLOB_RECURSE BASIC_SRC *.cpp)

# has to be dynamic, because we need to include all the symbols to
# load and link the bytecode. A static libraries would take only the
# symbols needed at linktime.
Add_Library(tlm-basic SHARED ${BASIC_SRC})
