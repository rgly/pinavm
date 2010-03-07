#!/bin/bash

trap 'echo "Error detected! End of script.";exit 1' ERR
#set -x

##############################################
################ CONFIG  #####################
##############################################

(cd .. && make config.sh) || exit 1
source ../config.sh

SCRIPT_DIR=`pwd`

# ##### INITIALIZATION #####
 test -d "$DOWNLOAD_AND_COMPILE_DIR" || \
     (echo "$DOWNLOAD_AND_COMPILE_DIR does not exist, creating it" && \
     mkdir -p "$DOWNLOAD_AND_COMPILE_DIR")

test -d "$INSTALL_PATH_LLVM" || \
    (echo "$INSTALL_PATH_LLVM does not exist, creating it" && \
    mkdir -p "$INSTALL_PATH_LLVM")

test -d "$INSTALL_PATH_SYSTEMC_LLVM" || \
    (echo "$INSTALL_PATH_SYSTEMC_LLVM does not exist, creating it" && \
    mkdir -p "$INSTALL_PATH_SYSTEMC_LLVM")

llvm_configure_flags="--prefix=$INSTALL_PATH_LLVM --enable-debug-runtime --disable-optimized --enable-checking --enable-bindings=none --enable-libffi=no"

###############################
########## LLVM ###############
###############################
install_llvm() {
    echo "Building llvm..."
    cd "$DOWNLOAD_AND_COMPILE_DIR"
    test -f llvm-2.6.tar.gz || wget http://llvm.org/releases/2.6/llvm-2.6.tar.gz
    tar xzf llvm-2.6.tar.gz
    cd llvm-2.6
    test -d objdir || mkdir objdir
    cd objdir
    ../configure ${llvm_configure_flags}
    make
    rm -rf "$INSTALL_PATH_LLVM"
    mkdir -p "$INSTALL_PATH_LLVM"
    make install
}

################################################
########## SYSTEMC (normal install) ############
################################################

patch_systemc() {
    patch -p0 < ${SCRIPT_DIR}/../systemc-2.2.0.patch

    ##### Link to Pinapa #########
    sed -i -e's/main(/launch_systemc(/' ./src/sysc/kernel/sc_main.cpp
    sed -i -e's/namespace sc_core {/extern void pinapa_callback();\nnamespace sc_core {/' \
        ./src/sysc/kernel/sc_simcontext.cpp
    sed -i -e's/context->simulate( *duration *);/pinapa_callback();/' \
	./src/sysc/kernel/sc_simcontext.cpp
}

install_systemc_gcc () {
    cd "$DOWNLOAD_AND_COMPILE_DIR"
    rm -fr systemc-2.2.0-gcc
    test -f systemc-2.2.0.tgz || wget http://www-verimag.imag.fr/~moy/cours/tlm/systemc/systemc-2.2.0.tgz
    tar xzf systemc-2.2.0.tgz
    mv systemc-2.2.0 systemc-2.2.0-gcc
    cd systemc-2.2.0-gcc

    patch_systemc

    rm -rf objdir
    mkdir objdir
    cd objdir
    chmod +x ../configure
    # most ./configure scripts create the target, but it seems SystemC's doesn't ...
    mkdir -p "$INSTALL_PATH_SYSTEMC_GCC"
    ../configure --prefix="$INSTALL_PATH_SYSTEMC_GCC"
    make pthreads_debug
    # SystemC's configure.in is buggy, and seems not to obey --prefix correctly ...
    make prefix="$INSTALL_PATH_SYSTEMC_GCC" install
}


###############################
########## LLVM-GCC ###########
###############################
install_llvm_gcc () {
    echo "Building llvm-gcc..."
    cd "$DOWNLOAD_AND_COMPILE_DIR"
    test -f "llvm-gcc-4.2-2.6-i686-linux.tar.gz" || \
	wget http://llvm.org/releases/2.6/llvm-gcc-4.2-2.6-i686-linux.tar.gz
    tar xzf llvm-gcc-4.2-2.6-i686-linux.tar.gz
    rm -rf "$INSTALL_PATH_LLVMGCC"
    mv llvm-gcc-4.2-2.6-i686-linux "$INSTALL_PATH_LLVMGCC"
}

####################################################
########## SYSTEMC (compiled with LLVM) ############
####################################################

install_systemc_llvm () {
    echo "Installing SystemC (patched and compiled with LLVM) ..."
    cd "$DOWNLOAD_AND_COMPILE_DIR"
    rm -fr systemc-2.2.0-llvm
    test -f systemc-2.2.0.tgz || cp ~marquet/local/download/systemc-2.2.0.tgz .
    tar xzf systemc-2.2.0.tgz
    mv systemc-2.2.0 systemc-2.2.0-llvm
    cd systemc-2.2.0-llvm

    patch_systemc

    export CXX="llvm-g++ --emit-llvm"
    export RANLIB="echo this is the old ranlib command on file :"

    ##### Remove qt (quick threads) from the list of subdirs to build ########
    rm -rf src/sysc/qt
    perl -pi -e 's/qt //' ./src/sysc/Makefile.am

    ##### Change 'ar' command to 'llvm-link' #####
    for file in $(find -name Makefile.am)
    do
	echo processing file "$file"
	toreplace='$(AR_TOOL) $(top_builddir)\/src\/$(MAIN_LIBRARY)'
	replacement='llvm-link -f -o $(top_builddir)\/src\/$(MAIN_LIBRARY) $(top_builddir)\/src\/$(MAIN_LIBRARY)'
	sed -i -e"s/$toreplace/$replacement/" "$file"
	(echo; echo 'AR = llvm-link') >> "$file"
	(echo; echo 'RANLIB = true') >> "$file"
	(echo; echo 'ARFLAGS = -f -o') >> "$file"
    done

    ##### Add llvm-g++ in the list of copatible compilers ###########
    perl -pi -e 's/c\+\+ | g\+\+/c\+\+ | g\+\+ | llvm-g\+\+/' ./configure.in
    perl -pi -e 's/ar cru/llvm-link -f -o/' ./configure.in
    perl -pi -e 's/ src\/sysc\/qt\/Makefile//' ./configure.in

    aclocal && autoconf && automake

    for file in $(find src/sysc/ -name Makefile.in)
    do
	echo processing file "$file"
	sed -i -e's/$(AR) cru/$(AR) $(ARFLAGS)/' "$file"
    done


    ##### configure, make, make install
    rm -rf objdir
    mkdir objdir
    cd objdir
    chmod +x ../configure
    ../configure --prefix="$INSTALL_PATH_SYSTEMC_LLVM"
    make pthreads_debug

    ###### Manual make install #######
    test -d "$INSTALL_PATH_SYSTEMC_LLVM" && rm -rf "$INSTALL_PATH_SYSTEMC_LLVM"
    mkdir -p "$INSTALL_PATH_SYSTEMC_LLVM"

    for libfile in $(find . -name "*.a")
    do
	llvm-link -f -o libsystemc.a "$libfile"
    done

    mkdir "$INSTALL_PATH_SYSTEMC_LLVM"/lib-linux
    cp libsystemc.a "$INSTALL_PATH_SYSTEMC_LLVM"/lib-linux/
    mkdir "$INSTALL_PATH_SYSTEMC_LLVM"/include
    cd ../src
    cp systemc systemc.h "$INSTALL_PATH_SYSTEMC_LLVM"/include/
    cd sysc
    mkdir "$INSTALL_PATH_SYSTEMC_LLVM"/include/sysc
    for hfile in $(find . -name "*.h")
    do
	DIRNAME=$(dirname "$hfile")
	DIRNAME_CORRECTED=$(echo "$DIRNAME" | sed s/'.\/'//)
	test -d "$INSTALL_PATH_SYSTEMC_LLVM/include/sysc/$DIRNAME_CORRECTED" || \
	    mkdir -p "$INSTALL_PATH_SYSTEMC_LLVM/include/sysc/$DIRNAME_CORRECTED"
	cp "$hfile" "$INSTALL_PATH_SYSTEMC_LLVM/include/sysc/$DIRNAME_CORRECTED"
    done
}

###############################
########### PINAPA ############
###############################
compile_pinapa () {
    echo "Building pinapa..."
    cd ${SRC_ROOT_DIR}/toplevel
    make
}

do_you_want () {
    echo "Do you want me to install it for you?"
    echo "y: Yes, install it now"
    echo "other: No, I'll install it myself."
    read answer
    if [ "$answer" = "y" ]; then
	"$@"
    else
	echo "aborting."
	exit 1
    fi
}

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



echo "$(basename $0) done."
