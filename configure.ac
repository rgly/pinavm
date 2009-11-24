################################################################################
#                         Copyright STMicroelectronics, Verimag
#    Reproduction and Communication of this document is strictly prohibited
#          unless specifically authorized in writing by STMicroelectronics
#-------------------------------------------------------------------------------
#                           System Platform Group -- HPC
#-------------------------------------------------------------------------------

# Process this file with autoconf to produce a configure script.

# Verifies that autoconf is at least version 2.59
AC_PREREQ(2.59)

# Initialization of the configure script
AC_INIT([pinapa],[1.0])

# Path to a source file of the project
AC_CONFIG_SRCDIR([doc/INSTALL])

# Where to find install.sh, config.sub, config.guess
AC_CONFIG_AUX_DIR(config)

# For retrieving the type of host for compilation
AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_CXXCPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_ARG_WITH(tools-dir,
	     AC_HELP_STRING([--with-tools-dir=...],
                   [Directory where external tools (SystemC, LLVM, ...) should be downloaded and installed.]),
	     [TOOLS_DIR="${withval}"],
             [AC_MSG_ERROR([Please specify --with-tools-dir=...])])

DOWNLOAD_AND_COMPILE_DIR="${TOOLS_DIR}/download"
INSTALL_PATH_LLVM=${TOOLS_DIR}lib/llvm-2.6
INSTALL_PATH_LLVMGCC=${TOOLS_DIR}lib/llvm-gcc
INSTALL_PATH_SYSTEMC_LLVM=${TOOLS_DIR}lib/systemc-2.2.0-llvm
INSTALL_PATH_SYSTEMC_GCC=${TOOLS_DIR}lib/systemc-2.2.0-gcc

AC_MSG_NOTICE(Summary of variables:

DOWNLOAD_AND_COMPILE_DIR: ${DOWNLOAD_AND_COMPILE_DIR}
INSTALL_PATH_LLVM: ${INSTALL_PATH_LLVM}
INSTALL_PATH_LLVMGCC: ${INSTALL_PATH_LLVMGCC}
INSTALL_PATH_SYSTEMC_LLVM: ${INSTALL_PATH_SYSTEMC_LLVM}
INSTALL_PATH_SYSTEMC_GCC: ${INSTALL_PATH_SYSTEMC_GCC}
)

AC_SUBST(DOWNLOAD_AND_COMPILE_DIR)
AC_SUBST(INSTALL_PATH_LLVM)
AC_SUBST(INSTALL_PATH_LLVMGCC)
AC_SUBST(INSTALL_PATH_SYSTEMC_LLVM)
AC_SUBST(INSTALL_PATH_SYSTEMC_GCC)

# Defining files to be generated
AC_CONFIG_FILES([config.mk
                 config.sh])
# Generating target files
AC_OUTPUT

