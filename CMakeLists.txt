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

# Config file generation
set(EXTERNAL_TOOLS_DIR "./external/downloaded/"
  CACHE
  PATH "Directory where external tools will be downloaded and compiled")
get_filename_component(ABS_TOOLS_DIR "${EXTERNAL_TOOLS_DIR}" ABSOLUTE)

set(DOWNLOAD_AND_COMPILE_DIR "${ABS_TOOLS_DIR}/download")
set(INSTALL_PATH_LLVM ${ABS_TOOLS_DIR}/lib/llvm-3.2)
set(INSTALL_PATH_LLVMGCC ${ABS_TOOLS_DIR}/lib/llvm-gcc)
set(INSTALL_PATH_SYSTEMC_LLVM ${ABS_TOOLS_DIR}/lib/systemc-2.2.0-llvm)
set(INSTALL_PATH_SYSTEMC_GCC ${ABS_TOOLS_DIR}/lib/systemc-2.2.0-gcc)

set(abs_top_srcdir ${CMAKE_SOURCE_DIR})

configure_file(config.sh.in config.sh)
configure_file(config.mk.in config.mk)

