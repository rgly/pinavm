#!/bin/sh

trap 'echo "Error detected! End of script.";exit 1' ERR

##############################################
################ CONFIG  #####################
##############################################
DOWNLOAD_AND_COMPILE_DIR=$HOME/local/download/essai3
INSTALL_PATH_LLVM=$HOME/local/lib/llvm-2.6
INSTALL_PATH_LLVMGCC=$HOME/local/lib/llvm-gcc
INSTALL_PATH_SYSTEMC=$HOME/local/lib/systemc-2.2.0-llvm


# ##### INITIALIZATION #####
 test -d $DOWNLOAD_AND_COMPILE_DIR || (echo "$DOWNLOAD_AND_COMPILE_DIR does not exist, creating it" && mkdir -p $DOWNLOAD_AND_COMPILE_DIR)

test -d $INSTALL_PATH_LLVM || (echo "$INSTALL_PATH_LLVM does not exist, creating it" && mkdir -p $INSTALL_PATH_LLVM)

test -d $INSTALL_PATH_SYSTEMC || (echo "$INSTALL_PATH_SYSTEMC does not exist, creating it" && mkdir -p $INSTALL_PATH_SYSTEMC)

###############################
########## LLVM ###############
###############################
echo Building llvm...
cd $DOWNLOAD_AND_COMPILE_DIR
test -f llvm-2.6.tar.gz || wget http://llvm.org/releases/2.6/llvm-2.6.tar.gz
tar xzf llvm-2.6.tar.gz
cd llvm-2.6
test -d objdir || mkdir objdir
cd objdir
../configure --prefix=$INSTALL_PATH_LLVM --enable-debug-runtime --disable-optimized --enable-checking --enable-bindings=none --enable-target=x86,simple
make
rm -rf $INSTALL_PATH_LLVM
mkdir -p $INSTALL_PATH_LLVM
make install

###############################
########## LLVM-GCC ###########
###############################
echo Building llvm-gcc...
cd $DOWNLOAD_AND_COMPILE_DIR
test -f "llvm-gcc-4.2-2.6-i686-linux.tar.gz" || wget http://llvm.org/releases/2.6/llvm-gcc-4.2-2.6-i686-linux.tar.gz
tar xzf llvm-gcc-4.2-2.6-i686-linux.tar.gz
rm -rf $INSTALL_PATH_LLVMGCC
mv llvm-gcc-4.2-2.6-i686-linux $INSTALL_PATH_LLVMGCC

###############################
########## SYSTEMC ############
###############################
cd $DOWNLOAD_AND_COMPILE_DIR
test -f systemc-2.2.0.tgz || cp ~marquet/local/download/systemc-2.2.0.tgz .
tar xzf systemc-2.2.0.tgz
mv systemc-2.2.0 systemc-2.2.0-llvm
cd systemc-2.2.0-llvm

export CXX="llvm-g++ --emit-llvm"
export RANLIB="echo this is the old ranlib command on file :"

##### Link to Pinapa #########
sed -i -e's/main(/launch_systemc(/' ./src/sysc/kernel/sc_main.cpp
sed -i -e's/namespace sc_core {/extern void pinapa_callback();\nnamespace sc_core {/' ./src/sysc/kernel/sc_simcontext.cpp

##### Remove qt (quick threads) from the list of subdirs to build ########
rm -rf src/sysc/qt
perl -pi -e 's/qt //' ./src/sysc/Makefile.am

##### Change 'ar' command to 'llvm-link' #####
for file in `find -name Makefile.am`
do
    echo processing file $file
    sed -i -e's/$(AR_TOOL) $(top_builddir)\/src\/$(MAIN_LIBRARY)/llvm-link -f -o $(top_builddir)\/src\/$(MAIN_LIBRARY) $(top_builddir)\/src\/$(MAIN_LIBRARY)/' $file
    (echo; echo 'AR = llvm-link') >> $file
    (echo; echo 'RANLIB = true') >> $file
    (echo; echo 'ARFLAGS = -f -o') >> $file
done

##### Add llvm-g++ in the list of copatible compilers ###########
perl -pi -e 's/c\+\+ | g\+\+/c\+\+ | g\+\+ | llvm-g\+\+/' ./configure.in
perl -pi -e 's/ar cru/llvm-link -f -o/' ./configure.in
perl -pi -e 's/ src\/sysc\/qt\/Makefile//' ./configure.in

aclocal && autoconf && automake

for file in `find src/sysc/ -name Makefile.in`
do
    echo processing file $file
    sed -i -e's/$(AR) cru/$(AR) $(ARFLAGS)/' $file
done


##### configure, make, make install
rm -rf objdir
mkdir objdir
cd objdir
chmod +x ../configure
../configure --prefix=$INSTALL_PATH_SYSTEMC
make pthreads_debug

###### Manual make install #######
test -d $INSTALL_PATH_SYSTEMC && rm -rf $INSTALL_PATH_SYSTEMC
mkdir -p $INSTALL_PATH_SYSTEMC

for libfile in `find . -name "*.a"`
do
    llvm-link -f -o libsystemc.a $libfile
done

mkdir $INSTALL_PATH_SYSTEMC/lib-linux
cp libsystemc.a $INSTALL_PATH_SYSTEMC/lib-linux/
mkdir $INSTALL_PATH_SYSTEMC/include
cd ../src
cp systemc systemc.h $INSTALL_PATH_SYSTEMC/include/
cd sysc
mkdir $INSTALL_PATH_SYSTEMC/include/sysc
for hfile in `find . -name "*.h"`
do
    DIRNAME=`dirname $hfile`
    DIRNAME_CORRECTED=`echo $DIRNAME | sed s/'.\/'//`
    test -d $INSTALL_PATH_SYSTEMC/include/sysc/$DIRNAME_CORRECTED || mkdir -p $INSTALL_PATH_SYSTEMC/include/sysc/$DIRNAME_CORRECTED
    cp $hfile $INSTALL_PATH_SYSTEMC/include/sysc/$DIRNAME_CORRECTED
done

# ../configure --prefix=/home/marquet/local/lib/llvm-gcc --program-prefix=llvm- --enable-llvm=/home/marquet/local/download/llvm-2.6/objdir --enable-languages=c,c++ && make && make install
