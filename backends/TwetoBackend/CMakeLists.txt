FILE(GLOB TWETO_SRC *.cpp)

Include_Directories(../../frontend)

Add_Library(tweto ${TWETO_SRC})
