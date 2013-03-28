/*
  This program is executed for CMake to detect the host architecture.
  It is used by systemc.cmake
*/
#if defined( __sparc ) || defined( __sparc__ )
#define ARCH "sparc"
#elif defined( __hppa )
#define ARCH "hppa"
#elif defined( __i386 )
#define ARCH "i386"
#elif defined( __ppc__ )
#define ARCH "powerpc_mach"
#elif defined( __x86_64__ )
#define ARCH "iX86_64"
#elif defined( __powerpc )
#define ARCH "powerpc_sys5"
#endif

#include <iostream>

int main(){
    std::cout << ARCH ;
    return 0;
}
