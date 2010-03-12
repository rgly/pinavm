#! /bin/echo please-dont-execute-this-file-but-source-it:

# true if $1 has a greater version number than $2.
version_greater () {
    major_1=$(echo "$1" | sed 's/\..*//')
    major_2=$(echo "$2" | sed 's/\..*//')
    if [ "$major_1" -lt "$major_2" ]; then
	return 1
    elif [ "$major_1" -gt "$major_2" ]; then
	return 0
    fi
    # Get rid of "svn" or similar suffixes.
    minor_1=$(echo "$1" | sed -e 's/.*\.//' -e 's/[^0-9]*$//')
    minor_2=$(echo "$2" | sed -e 's/.*\.//' -e 's/[^0-9]*$//')
    if [ "$minor_1" -lt "$minor_2" ]; then
	return 1
    fi
    return 0
}

test_version_greater_true () {
    version_greater "$1" "$2" || echo "FAIL: version_greater $1 $2"
}

test_version_greater_false () {
    version_greater "$1" "$2" && echo "FAIL: version_greater $1 $2"
}

test_version_greater () {
    test_version_greater_true 2.7svn 2.6
    test_version_greater_true 2.0 1.1
    test_version_greater_true 2.1 1.0
    test_version_greater_true 2.6 2.6
    test_version_greater_false 2.6 2.7
}

###############################
########## LLVM ###############
###############################
install_llvm() {
    test -d "$INSTALL_PATH_LLVM" || \
	(echo "$INSTALL_PATH_LLVM does not exist, creating it" && \
	mkdir -p "$INSTALL_PATH_LLVM")

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
    patch -p0 < ${PINAVM_DIR}/systemc-2.2.0.patch

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

build_systemc_for_llvm () {
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
    mkdir -p "$INSTALL_PATH_SYSTEMC_LLVM"

    llvm-link -f -o libsystemc.a $(find src/sysc/ -name "*.a")

    mkdir -p "$INSTALL_PATH_SYSTEMC_LLVM"/lib-linux
    mkdir -p "$INSTALL_PATH_SYSTEMC_LLVM"/include/sysc
    cp libsystemc.a "$INSTALL_PATH_SYSTEMC_LLVM"/lib-linux/

    cd ../src/
    # copy files keeping the directory structure
    cp systemc systemc.h "$INSTALL_PATH_SYSTEMC_LLVM"/include/
    find sysc/ -name "*.h" | tar -cf - -T - | \
	(cd "$INSTALL_PATH_SYSTEMC_LLVM/include/"; tar -xf -)
}

install_systemc_llvm () {
    echo "Installing SystemC (patched and compiled with LLVM) ..."
    cd "$DOWNLOAD_AND_COMPILE_DIR"
    rm -fr systemc-2.2.0-llvm
    test -f systemc-2.2.0.tgz || wget http://www-verimag.imag.fr/~moy/systemc-2.2.0.tgz -O systemc-2.2.0.tgz
    tar xzf systemc-2.2.0.tgz
    mv systemc-2.2.0 systemc-2.2.0-llvm
    cd systemc-2.2.0-llvm

    patch_systemc

    build_systemc_for_llvm
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
