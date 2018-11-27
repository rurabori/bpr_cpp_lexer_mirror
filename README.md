# BPR_CPP_LEXER

This does not compile with anything less than GCC-9 (as of right now that means trunk).

My compiler config : 
	Using built-in specs.
	COLLECT_GCC=g++
	COLLECT_LTO_WRAPPER=/usr/gcc-trunk/libexec/gcc/x86_64-pc-linux-gnu/9.0.0/lto-wrapper
	Target: x86_64-pc-linux-gnu
	Configured with: ../gcc/configure --prefix=/usr/gcc-trunk --enable-languages=c,c++,fortran --disable-libquadmath --disable-libquadmath-support --disable-werror --disable-bootstrap --enable-gold --disable-multilib : (reconfigured) ../gcc/configure --prefix=/usr/gcc-trunk --enable-languages=c,c++,fortran --disable-libquadmath --disable-libquadmath-support --disable-werror --disable-bootstrap --enable-gold --disable-multilib
	Thread model: posix
	gcc version 9.0.0 20181118 (experimental) (GCC)

	Built from commit 20f6624d3e5.

	EXPERIMENTAL_GCC_9 is there from Hanka Dusikova because GCC does not implement cnttp in a way that works with ctre so some code is 'ifdefed'.

	commandline to compile main : g++ -std=c++2a -DEXPERIMENTAL_GCC_9 -I'compile-time-regular-expressions/include' -O3 -Wno-gnu-string-literal-operator-template  -pedantic -Wall -Wextra  -MMD -c main.cpp -o a.o.