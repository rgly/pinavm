FILE(GLOB FRONTEND_SRC *.cpp)

Include_Directories(../external/systemc-2.2.0/src)

Add_Library(frontend ${FRONTEND_SRC})
