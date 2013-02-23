FILE(GLOB PROMELA_SRC *.cpp)

Include_Directories(../../frontend)

needs_no_rtti(PromelaTargetMachine.cpp)
needs_no_rtti(PromelaWriter.cpp)

Add_Library(promela ${PROMELA_SRC})
