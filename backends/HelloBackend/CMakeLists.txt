FILE(GLOB HELLO_SRC *.cpp)
Include_Directories(${CMAKE_SOURCE_DIR}/frontend)
Add_Library(hello ${HELLO_SRC})
