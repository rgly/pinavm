LLVM=
LLVM=/home/moy/local/usr-squeeze/src/llvm-mirror/Debug+Asserts/bin/

run: asm.bc libtest.so
	$(LLVM)lli -load=./libtest.so asm.bc

asm.bc: asm.ll
	$(LLVM)llvm-as $< -o $@

libtest.so: libtest.c libtest.h
	gcc -fpic -shared $< -o $@

clean:
	-$(RM) asm.bc libtest.so
