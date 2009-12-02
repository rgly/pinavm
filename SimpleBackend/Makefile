##===- lib/Target/CBackend/Makefile ------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../../
LIBRARYNAME = LLVMSimpleBackend
#LOADABLE_MODULE = 1
#LINK_COMPONENTS=
KEEP_SYMBOLS=1
CPPFLAGS=-I$(SYSTEMCFRONTEND) -I$(SYSTEMCROOT)/include
CXXFLAGS=-g -I$(SYSTEMCFRONTEND) -I$(SYSTEMCROOT)/include
#TARGET=simple

DIRS = TargetInfo

include $(LEVEL)/Makefile.common

#CompileCommonOpts += -Wno-format
