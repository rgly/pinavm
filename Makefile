##===- lib/Transforms/Hello/Makefile -----------------------*- Makefile -*-===##
# 
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
# 
##===----------------------------------------------------------------------===##

LEVEL = ../../local/download/llvm-2.4/objdir
LIBRARYNAME = pinapa
LOADABLE_MODULE = 1
USEDLIBS =
LINK_COMPONENTS=
KEEP_SYMBOLS=1
CXXFLAGS=-g

include $(LEVEL)/Makefile.common
