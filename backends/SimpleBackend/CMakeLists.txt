FILE(GLOB SIMPLE_SRC *.cpp)

Include_Directories(${CMAKE_SOURCE_DIR}/frontend)

needs_no_rtti(SimpleTargetMachine.cpp)
needs_no_rtti(SimpleWriter.cpp)
needs_no_rtti(SimpleBackendNameAllUsedStructsAndMergeFunctions.cpp)

Add_Library(simple ${SIMPLE_SRC})
