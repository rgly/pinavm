configure_file(GetRootPath.cpp.in GetRootPath.cpp)

# Should include all the configured file.
FILE(GLOB UTILS_SRC *.cpp ${CMAKE_CURRENT_BINARY_DIR}/*.cpp)

Add_Library(utils ${UTILS_SRC} )
