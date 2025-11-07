# Building the Modified GCC Cross Compiler Manually

## Introduction

At the time of writing, jOSh uses a cross compiler based on a modified GCC 13.3.0 + binutils 2.42. The compiler has been modified to add multilib support to the x86_64-elf target in order to:

 - Provide a 32-bit (`-m32`) version of libgcc for building the module loader.
 - [Compile libgcc without the red zone](https://wiki.osdev.org/Libgcc_without_red_zone)[^1].
 - Provide a version of libgcc with `-mcmodel=large`. This must be mutually exclusive with `-m32`.
 
[^1]: It may not strictly be necessary at this stage to do this via multilib, since jOSh does not use crtbegin.o or crtend.o. Adding `-mno-red-zone` to `CFLAGS_FOR_TARGET` may be sufficient in this case, but doing this via multilib now may save a headache later. See [the discussion page](https://wiki.osdev.org/Talk:Libgcc_without_red_zone) for more information.

The details of how to patch in multilib support for this target can be found [on the OSDev wiki](https://wiki.osdev.org/Libgcc_without_red_zone). This is taken further for jOSh by adding `m32` and `mcmodel=large` as multilib options, and adding an exception for `m32`/`mcmodel=large`.

## Patching the Compiler

These steps target GCC 13.3.0, but should be valid for any recent version of GCC.

 1) Download the source code for [GNU Binutils 2.42](ftp://www.mirrorservice.org/sites/ftp.gnu.org/gnu/binutils/binutils-2.42.tar.xz) and [GCC 13.3.0](ftp://www.mirrorservice.org/sites/ftp.gnu.org/gnu/gcc/gcc-13.3.0/gcc-13.3.0.tar.xz) and extract both. You should now have two directories in your workdir: `gcc-13.3.0` and `binutils-2.42`.
 2) Create the new file `gcc-13.3.0/gcc/config/i386/t-x86_64-elf` with the following content:
```

MULTILIB_OPTIONS += m32 mno-red-zone mcmodel=large
MULTILIB_DIRNAMES += m32 no-red-zone mcmodel-large
MULTILIB_EXCEPTIONS += *m32/*mcmodel=large*
```
 3) Modify the file `gcc-13.3.0/gcc/config.gcc`. Find the lines:
```
    tm_file="${tm_file} i386/unix.h i386/att.h elfos.h newlib-stdint.h i386/i386elf.h i386/x86-64.h"
 	;;
```
 and add the following line above `tm_file=...`:
```
	tmake_file="${tmake_file} i386/t-x86_64-elf"
```
 4) Export the following environment variables. Set the `PREFIX` variable to wherever you want the cross compiler to be installed:
```bash
export PREFIX="$HOME/jOSh-cross"
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"
```
 5) Compile binutils as normal with the following:
```bash
mkdir build-binutils && cd build-binutils
../binutils-2.42/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror --enable-multilib
make -j
make install
cd ..
```
 6) Compile GCC. Note the `CFLAGS_FOR_TARGET` variable used for libgcc.
```bash
mkdir build-gcc && cd build-gcc
../gcc-13.3.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers --disable-hosted-libstdcxx --enable-multilib
make -j all-gcc
make CFLAGS_FOR_TARGET="-fno-reorder-functions -fno-section-anchors -mno-sse3 -mno-sse4" -j all-target-libgcc
make -j all-target-libstdc++-v3
make install-gcc
make install-target-libgcc
make install-target-libstdc++-v3
cd ..
```
 7) The compiler should now be built in `~/jOSh-cross`. You can either point jOSh's Makefiles to this using the standard variables.

For GCC 13.3.0, you can use the following patch file instead of doing steps 2 and 3 manually. Apply it with `patch -p0 < patch.diff`:
```
diff -urN gcc-13.3.0/gcc/config/i386/t-x86_64-elf gcc-13.3.0-patched/gcc/config/i386/t-x86_64-elf
--- gcc-13.3.0/gcc/config/i386/t-x86_64-elf	1970-01-01 01:00:00.000000000 +0100
+++ gcc-13.3.0-patched/gcc/config/i386/t-x86_64-elf	1970-01-01 01:00:00.000000000 +0100
@@ -0,0 +1,4 @@
+
+MULTILIB_OPTIONS += m32 mno-red-zone mcmodel=large
+MULTILIB_DIRNAMES += m32 no-red-zone mcmodel-large
+MULTILIB_EXCEPTIONS += *m32/*mcmodel=large*
\ No newline at end of file
diff -urN gcc-13.3.0/gcc/config.gcc gcc-13.3.0-patched/gcc/config.gcc
--- gcc-13.3.0/gcc/config.gcc	2024-05-21 08:47:38.000000000 +0100
+++ gcc-13.3.0-patched/gcc/config.gcc	2024-05-21 08:47:38.000000000 +0100
@@ -1866,6 +1866,7 @@
 	tm_file="${tm_file} i386/unix.h i386/att.h elfos.h newlib-stdint.h i386/i386elf.h"
 	;;
 x86_64-*-elf*)
+	tmake_file="${tmake_file} i386/t-x86_64-elf"
 	tm_file="${tm_file} i386/unix.h i386/att.h elfos.h newlib-stdint.h i386/i386elf.h i386/x86-64.h"
 	;;
 x86_64-*-rtems*)
```
