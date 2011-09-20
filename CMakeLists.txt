cmake_minimum_required(VERSION 2.8)

project(PinaVM CXX)

# In theory, we could use the package discovery like this:
# SET(CMAKE_MODULE_PATH "${LLVM_DIR}")
# find_package(LLVM)
# (see http://llvm.org/docs/CMake.html#embedding )
# In practice, I didn't manage to get llvm_map_components_to_libraries
# to work, hence we do it by calling llvm-config ourselves instead.

execute_process(COMMAND llvm-config --cxxflags jit interpreter nativecodegen bitreader selectiondag
                OUTPUT_VARIABLE LLVM_CXXFLAGS)
# CMake doesn't like newline in CXXFLAGS, and there's at least a
# trailing \n in llvm-config's output.
STRING(REGEX REPLACE "\n" " " LLVM_CXXFLAGS "${LLVM_CXXFLAGS}")
# llvm-config has a -fno-rtti -fno-exception by default, but PinaVM
# uses dynamic_cast relatively heavily, including in complex
# multiple-inheritance contexts where LLVM's template-based
# replacement dyn_cast does not work.
STRING(REGEX REPLACE "-fno-rtti" " " LLVM_CXXFLAGS "${LLVM_CXXFLAGS}")
STRING(REGEX REPLACE "-fno-exceptions" " " LLVM_CXXFLAGS "${LLVM_CXXFLAGS}")

execute_process(COMMAND llvm-config --ldflags
                OUTPUT_VARIABLE LLVM_LDFLAGS)
STRING(REGEX REPLACE "\n" " " LLVM_LDFLAGS "${LLVM_LDFLAGS}")

# Some files define classes that inherit from LLVM classes, which are
# compiled with -fno-rtti as of svn version 98434 (14 mach 2010) while
# we do need -frtti (see above). Deriving a rtti class from a no-rtti
# doesn't link. So, these files are compiled without rtti.
macro(needs_no_rtti FILE)
  SET_SOURCE_FILES_PROPERTIES(${FILE} PROPERTIES COMPILE_FLAGS "-fno-rtti -fno-exceptions") 
endmacro()

execute_process(COMMAND llvm-config --libs jit interpreter nativecodegen bitreader selectiondag
                OUTPUT_VARIABLE LLVM_LIBS)

# turn -lfoo -lbar into foo;bar, so that CMake considers it as a list
STRING(REGEX REPLACE "\n" "" LLVM_LIBS "${LLVM_LIBS}")
STRING(REGEX REPLACE "^-l" "" LLVM_LIBS "${LLVM_LIBS}")
STRING(REGEX REPLACE " -l" ";" LLVM_LIBS "${LLVM_LIBS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CXXFLAGS}")
set(CMAKE_EXE_LINKER_FLAGS " ${CMAKE_EXE_LINKER_FLAGS} ${LLVM_LDFLAGS}")

# subdirectories
include_directories(${CMAKE_SOURCE_DIR}/external/systemc-2.2.0/src)
include_directories(${CMAKE_SOURCE_DIR}/external/basic/)
include_directories(${CMAKE_SOURCE_DIR}/external/TLM-2009-07-15/include/tlm)
include_directories(${CMAKE_SOURCE_DIR}/utils)

include(./systemc.cmake)
add_subdirectory(toplevel)
add_subdirectory(frontend)
add_subdirectory(utils)
add_subdirectory(external/basic)

add_subdirectory(backends/42Backend/)
add_subdirectory(backends/PromelaBackend/)
add_subdirectory(backends/SimpleBackend/)
add_subdirectory(backends/TwetoBackend/)

# Config file generation
set(EXTERNAL_TOOLS_DIR "./external/downloaded/"
  CACHE
  PATH "Directory where external tools will be downloaded and compiled")
get_filename_component(ABS_TOOLS_DIR "${EXTERNAL_TOOLS_DIR}" ABSOLUTE)

set(DOWNLOAD_AND_COMPILE_DIR "${ABS_TOOLS_DIR}/download")
set(INSTALL_PATH_LLVM ${ABS_TOOLS_DIR}/lib/llvm-2.8)
set(INSTALL_PATH_LLVMGCC ${ABS_TOOLS_DIR}/lib/llvm-gcc)
set(INSTALL_PATH_SYSTEMC_LLVM ${ABS_TOOLS_DIR}/lib/systemc-2.2.0-llvm)
set(INSTALL_PATH_SYSTEMC_GCC ${ABS_TOOLS_DIR}/lib/systemc-2.2.0-gcc)

set(abs_top_srcdir ${CMAKE_SOURCE_DIR})

configure_file(config.sh.in config.sh)
configure_file(config.mk.in config.mk)
