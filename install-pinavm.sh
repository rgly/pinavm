#!/bin/bash

trap 'echo "Error detected! End of script.";exit 1' ERR
set -x


SCRIPT_DIR=$(cd "$(dirname $0)"; pwd)
PINAVM_DIR=$SCRIPT_DIR
source "$PINAVM_DIR"/scripts/install-lib.sh

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
PATH="$INSTALL_PATH_LLVMGCC/bin:${PATH}"
PATH="$INSTALL_PATH_LLVM/bin:${PATH}"
export PATH


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
    echo "you can install it yourself (aptitude install llvm-snapshot does the"
    echo "trick on Debian systems), or let me do it (but it takes a long time)"
    do_you_want install_llvm
fi

echo "Using llvm-config: $(which llvm-config)"

if ! [ "$(llvm-config --version | sed 's/svn//')" = 3.2 ]; then
    echo "LLVM's version is $(llvm-config --version), while I expected 3.2 or 3.2svn."
    echo "It's unlikely that anything work unless LLVM >= 3.2svn is installed"
    echo "(and the corresponding llvm-config executable be at the front of your PATH)."
    do_you_want install_llvm
fi

# Build PinaVM
cd "$SCRIPT_DIR"
# We probably just installed llvm-config, and CMake needs it
cmake .
make

date
echo "

$(basename $0) done.

If the script installed llvm-gcc and llvm, you need to add
the following to your shell's config file (~/.bashrc or so):

PATH=\"$INSTALL_PATH_LLVMGCC/bin:\${PATH}\"
PATH=\"$INSTALL_PATH_LLVM/bin:\${PATH}\"
export PATH

You can now try to compile and run an example with

cd $PINAVM_DIR/systemc-examples/jerome-chain
make promela"
