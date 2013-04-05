# Make sure there is a LLVM_CONFIG
if(NOT DEFINED LLVM_ROOT)
  message(FATAL_ERROR "LLVM_ROOT should exists here. it is a bug."
                        " please contact authors.")
endif()

# We incorporate the CMake modules provided by LLVM:
# Using include(foo.cmake) searches CMAKE_MODULE_PATH,
# but find_package(bar) searches CMAKE_PREFIX_PATH
# so we must append llvm cmake module dir to both variables.
LIST(APPEND CMAKE_MODULE_PATH ${LLVM_ROOT}/share/llvm/cmake)
LIST(APPEND CMAKE_PREFIX_PATH ${LLVM_ROOT}/share/llvm/cmake)

# Not using version check in find_package is that llvm version is 3.2svn,
# which is different to 3.2 of debian package pool.
# So checks version later.

# The llvm's cmake module only contained in the LLVM which is build
# through CMake. Package pool of most linux distributions do not contained
# this module. This command try the CMake module of LLVM.
find_package(LLVM QUIET)

if(${LLVM_FOUND})
  llvm_map_components_to_libraries(LLVM_LIBS ${NEED_LLVM_LIB})
endif()
