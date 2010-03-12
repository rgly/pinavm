test.bc: test.cpp
	llvm-g++ -o $@ --emit-llvm -c $<

a.out.bc: test.bc
	llvm-ld -lstdc++ -disable-opt $<

run-lli: a.out.bc
	lli -load=/net/bauges/local/moy/usr/src/llvm-gcc-4.2-2.6-i686-linux/lib/libstdc++.so $<

clean:
	$(RM) *.bc *.ll
