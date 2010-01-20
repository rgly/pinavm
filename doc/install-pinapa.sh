#!/bin/sh

trap 'echo "Error detected! End of script.";exit 1' ERR
set -x

##############################################
################ CONFIG  #####################
##############################################

(cd .. && make config.sh)
source ../config.sh

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

###############################
########## BACKEND ############
###############################

install_backend() {
    echo "Building backend..."
    cd "$DOWNLOAD_AND_COMPILE_DIR"
    cd llvm-2.6
    test -L lib/Target/SimpleBackend || ln -s "$SRC_ROOT_DIR"/SimpleBackend/ lib/Target/SimpleBackend

    patch -p0 <<\EOF
--- configure 	2009-12-02 17:40:52.000000000 +0100
+++ configure	2009-12-02 17:43:40.000000000 +0100
@@ -5052,6 +5052,7 @@
         cbe)      TARGETS_TO_BUILD="CBackend $TARGETS_TO_BUILD" ;;
         msil)     TARGETS_TO_BUILD="MSIL $TARGETS_TO_BUILD" ;;
         cpp)      TARGETS_TO_BUILD="CppBackend $TARGETS_TO_BUILD" ;;
+        simple)   TARGETS_TO_BUILD="SimpleBackend $TARGETS_TO_BUILD" ;;
         *) { { echo "$as_me:$LINENO: error: Unrecognized target $a_target" >&5
 echo "$as_me: error: Unrecognized target $a_target" >&2;}
    { (exit 1); exit 1; }; } ;;
--- CMakeLists.txt      2009-08-18 17:29:35.000000000 +0200
+++ CMakeLists.txt      2009-12-02 17:28:08.000000000 +0100
@@ -60,6 +60,7 @@
   SystemZ
   X86
   XCore
+  SimpleBackend
   )
 
 if( MSVC )
EOF
    cd objdir
    ../configure ${llvm_configure_flags} --enable-targets=x86,simple
    make
    make install
}

################################################
########## SYSTEMC (normal install) ############
################################################

patch_systemc() {
    patch -p0 < ../systemc-2.2.0.patch

    patch -p0 <<\EOF
--- src/sysc/utils/sc_utils_ids.cpp     2006-12-15 21:31:39.000000000 +0100
+++ src/sysc/utils/sc_utils_ids.cpp     2009-11-04 15:05:00.000000000 +0100
@@ -58,8 +58,10 @@
 // the source.
 //
 
+#include <cstdlib>
+#include <string.h>
 #include "sysc/utils/sc_report.h"
-
+using namespace std;
 
 namespace sc_core {
 #define SC_DEFINE_MESSAGE(id,unused,text) extern const char id[] = text;
EOF

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
    test -f systemc-2.2.0.tgz || cp ~marquet/local/download/systemc-2.2.0.tgz .
    tar xzf systemc-2.2.0.tgz
    mv systemc-2.2.0 systemc-2.2.0-gcc
    cd systemc-2.2.0-gcc

    patch_systemc

    rm -rf objdir
    mkdir objdir
    cd objdir
    chmod +x ../configure
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

install_llvm
install_llvm_gcc
install_systemc_gcc
install_backend

( install_systemc_llvm )


echo "$(basename $0) done."
