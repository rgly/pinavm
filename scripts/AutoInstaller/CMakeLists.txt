cmake_minimum_required(VERSION 2.8.12)

# GCC_INSTALL_PREFIX
#   This flag denote the gcc installation to let Clang uses stdc++ from it.
#
# CMAKE_PREFIX_PATH
#   This flag denote the place to search tools, like python.

set(AUTOINSTALLER_DIR ${CMAKE_CURRENT_SOURCE_DIR})
include(${AUTOINSTALLER_DIR}/installLLVM.cmake)
autoinstall_llvm(${LLVM_RECOMMEND_VERSION} ${LLVM_ROOT} ${CMAKE_CURRENT_BINARY_DIR})
