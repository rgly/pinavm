FILE(GLOB PROMELA_SRC *.cpp)

Include_Directories(${CMAKE_SOURCE_DIR}/frontend)

needs_no_rtti(PromelaTargetMachine.cpp)
needs_no_rtti(PromelaWriter.cpp)

Add_Library(promela ${PROMELA_SRC})
