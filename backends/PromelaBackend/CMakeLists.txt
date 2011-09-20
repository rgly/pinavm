FILE(GLOB PROMELA_SRC *.cpp)

Include_Directories(../../frontend)

needs_no_rtti(PromelaTargetMachine.cpp)

Add_Library(promela ${PROMELA_SRC})
