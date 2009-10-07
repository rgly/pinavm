##### Set up #####
SYSTEMCLLVM_INSTALL_PATH=~/local/lib/systemc-2.2.0-llvm/
TEMPFILE=tempfile

export RANLIB="echo this is the old ranlib command on file :"

cp -r systemc-2.2.0 systemc-2.2.0-llvm

cd systemc-2.2.0-llvm
rm -rf objdir
mkdir objdir
cd objdir

##### Add llvm-g++ in the list of copatible compilers ###########
rm $TEMPFILE
touch $TEMPFILE
cat ../configure  | sed s/'c++ | g++'/'c++ | g++ | llvm-g++'/ >> $TEMPFILE
echo mv -f $TEMPFILE ../configure
mv -f $TEMPFILE ../configure

##### Remove qt (quick threads) from the list of subdirs to build ########
rm $TEMPFILE
touch $TEMPFILE
cat ../src/sysc/Makefile.in | sed s/'qt '//  >> $TEMPFILE
echo mv -f $TEMPFILE ../configure
mv -f $TEMPFILE ../src/sysc/Makefile.in

##### Replace the 'ar' tool by 'llvm-link' #########
FILES="../configure ../src/sysc/kernel/Makefile* ../src/sysc/tracing/Makefile* ../src/sysc/utils/Makefile* ../src/sysc/datatypes/misc/Makefile* ../src/sysc/datatypes/int/Makefile* ../src/sysc/datatypes/fx/Makefile* ../src/sysc/datatypes/bit/Makefile* ../src/sysc/communication/Makefile*"

for file in $FILES
  do
  echo processing file $file
  rm $TEMPFILE
  touch $TEMPFILE
  cat $file | sed s/'ar cru'/'llvm-link -f -o'/ | sed s/'$(AR) cru'/'llvm-link -f -o'/ >> $TEMPFILE
  echo mv -f $TEMPFILE $file 
  mv -f $TEMPFILE $file 
done

##### configure, make, make install
chmod +x ../configure
../configure --prefix=$SYSTEMCLLVM_INSTALL_PATH
make pthreads
rm -rf $SYSTEMCLLVM_INSTALL_PATH
make install
