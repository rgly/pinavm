cmake_minimum_required(VERSION 2.8.8)

project(PinaVM CXX)

# Find and set LLVM.
include(${CMAKE_SOURCE_DIR}/scripts/findLLVM.cmake)

set(BACKENDS Tweto 42 Promela Simple)

# Some files define classes that inherit from LLVM classes, which are
# compiled with -fno-rtti as of svn version 98434 (14 mach 2010) while
# we do need -frtti (see above). Deriving a rtti class from a no-rtti
# doesn't link. So, these files are compiled without rtti.
macro(needs_no_rtti FILE)
  SET_SOURCE_FILES_PROPERTIES(${FILE} PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions") 
endmacro()

# sets debug level to g3, which contains more infomation than g2.
# to use it : cmake /pinavm/source/ -DCMAKE_BUILD_TYPE=DEBUG
set(CMAKE_CXX_FLAGS_DEBUG "-g3")

# subdirectories
include_directories(${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/src)
include_directories(${CMAKE_SOURCE_DIR}/external/basic/)
include_directories(${CMAKE_SOURCE_DIR}/external/TLM-2009-07-15/include/tlm)
include_directories(${CMAKE_SOURCE_DIR}/utils)

include(${CMAKE_SOURCE_DIR}/scripts/systemc.cmake)
add_subdirectory(toplevel)
add_subdirectory(frontend)
add_subdirectory(utils)
add_subdirectory(external/basic)

# add backends
foreach(backend ${BACKENDS})
  add_subdirectory(backends/${backend}Backend/)
endforeach(backend)

string(REPLACE ";" " " LLVMC_FLAGS_SPACE "${LLVMC_FLAGS}")
configure_file(config.sh.in config.sh)
configure_file(config.mk.in config.mk)

