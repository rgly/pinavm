FILE(GLOB QUARANTEDEUX_SRC *.cpp)

Include_Directories(../../frontend)

needs_no_rtti(42TargetMachine.cpp)

Add_Library(quarantedeux ${QUARANTEDEUX_SRC})
