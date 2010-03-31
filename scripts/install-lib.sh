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
    svn checkout http://llvm.org/svn/llvm-project/llvm/branches/release_27/ llvm-2.7
    cd llvm-2.7
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

install_systemc_gcc () {
    cd "$PINAVM_DIR"/external/systemc-2.2.0

    ./autogen.sh

    rm -rf objdir-gcc
    mkdir objdir-gcc
    cd objdir-gcc

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
    case $(uname) in
	Darwin)
	    LLVM_GCC_BASE=llvm-gcc-4.2-2.6-i386-darwin9
	    LLVM_GCC_TAR="$LLVM_GCC_BASE".tar.gz
	    LLVM_GCC_URL=http://llvm.org/releases/2.6/$LLVM_GCC_TAR
	    ;;
	Linux)
	    LLVM_GCC_BASE=llvm-gcc-4.2-2.6-i686-linux
	    LLVM_GCC_TAR="$LLVM_GCC_BASE".tar.gz
	    LLVM_GCC_URL=http://llvm.org/releases/2.6/$LLVM_GCC_TAR
	    ;;
	*)
	    echo "Unsupported platform $(uname), sorry"
	    exit
    esac
    
    echo "Building llvm-gcc..."
    cd "$DOWNLOAD_AND_COMPILE_DIR"
    test -f "LLVM_GCC_TAR" || \
	wget "$LLVM_GCC_URL"
    tar xzf "$LLVM_GCC_TAR"
    rm -rf "$INSTALL_PATH_LLVMGCC"
    mv "$LLVM_GCC_BASE" "$INSTALL_PATH_LLVMGCC"
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

# deprecated.
install_systemc_llvm () {
    echo "Installing SystemC (patched and compiled with LLVM) ..."
    cd "$DOWNLOAD_AND_COMPILE_DIR"
    rm -fr systemc-2.2.0-llvm
    test -f systemc-2.2.0.tgz || wget http://www-verimag.imag.fr/~moy/systemc-2.2.0.tgz -O systemc-2.2.0.tgz
    tar xzf systemc-2.2.0.tgz
    mv systemc-2.2.0 systemc-2.2.0-llvm
    cd systemc-2.2.0-llvm

    build_systemc_for_llvm
}

###############################
########### PINAVM ############
###############################
compile_pinavm () {
    echo "Building PinaVM..."
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
