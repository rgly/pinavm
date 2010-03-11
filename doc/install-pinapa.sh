#!/bin/bash

trap 'echo "Error detected! End of script.";exit 1' ERR
#set -x


SCRIPT_DIR=$(cd "$(dirname $0)"; pwd)
PINAVM_DIR=$(cd "$SCRIPT_DIR"/..; pwd)
source "$SCRIPT_DIR"/install-lib.sh

##############################################
################ CONFIG  #####################
##############################################

(cd "$PINAVM_DIR" && make config.sh) || exit 1
source "$PINAVM_DIR"/config.sh

# Make sure llvm-config and llvm-g++ are in the $PATH.
# We ensure this at the start of the script so that
# re-running the script after installing llvm-gcc works.
# The user will need anyway to set his $PATH properly
# but there's a hint for that at the end of the script.
PATH="${PATH}:$INSTALL_PATH_LLVMGCC/bin"
PATH="${PATH}:$INSTALL_PATH_LLVM/bin"
export PATH

# ##### INITIALIZATION #####
test -d "$DOWNLOAD_AND_COMPILE_DIR" || \
     (echo "$DOWNLOAD_AND_COMPILE_DIR does not exist, creating it" && \
     mkdir -p "$DOWNLOAD_AND_COMPILE_DIR")

test -d "$INSTALL_PATH_SYSTEMC_LLVM" || \
    (echo "$INSTALL_PATH_SYSTEMC_LLVM does not exist, creating it" && \
    mkdir -p "$INSTALL_PATH_SYSTEMC_LLVM")

test -d "$INSTALL_PATH_SYSTEMC_GCC" || \
    (echo "$INSTALL_PATH_SYSTEMC_GCC does not exist, creating it" && \
    mkdir -p "$INSTALL_PATH_SYSTEMC_GCC")

llvm_configure_flags="--prefix=$INSTALL_PATH_LLVM --enable-debug-runtime --disable-optimized --enable-checking --enable-bindings=none --enable-libffi=no"

# TODO: all this tests should probably be moved to the configure
# script.
if ! llvm-gcc --version > /dev/null; then
    echo "llvm-gcc doesn't seem to be installed on your system."
    echo "you can install it yourself (aptitude install llvm-gcc does the"
    echo "trick on Debian systems), or let me do it."
    do_you_want install_llvm_gcc
fi

# Debian's llvm-config give /usr/include/llvm while hand-compiled
# llvm-config gives the path without llvm/.
if [ ! -r "$(llvm-config --includedir)/llvm/LLVMContext.h" ] && \
    [ ! -r "$(llvm-config --includedir)/LLVMContext.h" ] ; then
    echo "LLVM doesn't seem to be installed on your system."
    echo "you can install it yourself (aptitude install llvm-dev does the"
    echo "trick on Debian systems), or let me do it (but it takes a long time)"
    do_you_want install_llvm
fi

if [ "$(llvm-config --version)" != 2.6 ]; then
    echo "LLVM's version isn't 2.6. It's unlikely that anything work unless"
    echo "LLVM 2.6 is installed (and the corresponding llvm-config executable"
    echo "be at the front of your PATH)."
    do_you_want install_llvm
fi

install_systemc_gcc
( install_systemc_llvm )
compile_pinapa

date
echo "

$(basename $0) done.

If the script installed llvm-gcc and llvm, you need to add
the following to your shell's config file (~/.bashrc or so):

PATH=\"\${PATH}:$INSTALL_PATH_LLVMGCC/bin\"
PATH=\"\${PATH}:$INSTALL_PATH_LLVM/bin\"
export PATH

You can now try to compile and run an example with

cd $PINAVM_DIR/systemc-examples/jerome-chain
make promela"
