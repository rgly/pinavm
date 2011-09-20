FILE(GLOB SIMPLE_SRC *.cpp)

Include_Directories(../../frontend)

needs_no_rtti(SimpleTargetMachine.cpp)

Add_Library(simple ${SIMPLE_SRC})
