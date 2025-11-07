# jOSh

jOSh is a monolithic hobby kernel predominantly written in C. At present, it targets legacy boot x86-64, though it is hoped that support may be extended to UEFI, and later to other architectures. Its main purpose is to help me learn how an operating system works by re-implementing one from scratch. It is currently in a _very_ early stage of development, and does not yet provide essential features needed for running programs. However, it is in a continual state of development, and I hope it will eventually be a fully capable OS.

## Building

_Note: jOSh currently only supports x86-64! Many platform-specific features will be missing on other architectures, and compilation will almost certainly fail._

### Dependencies

 * GRUB
 * xorriso
 * make

jOSh currently uses a modified GCC cross-compiler to build. You can download the latest version of the cross-compiler and corresponding binutils used for compiling jOSh [here](https://github.com/joshbarrass/jOSh-gcc/releases). This compiler has been modified to compile libgcc for both 64-bit and 32-bit modes, and to ensure various features are disabled which would otherwise be unsafe to use in the compiler. If you want to build the cross-compiler yourself, see [Building the Modified GCC Cross Compiler Manually](docs/Building_cross_compiler.md). The compiler can be extracted directly to the source dir (jOSh's Makefiles will look for it under `./x86_64-elf-gcc-13.3.0`), or you can pass the `CC`, `CXX`, `STRIP` and `AR` variables to make to tell it where to find `gcc`, `g++`, `strip` and `ar`.

With the cross-compiler configured, jOSh can be built with:

```
make install-headers && make -j
```
This will create the sysroot in the source directory and install all header files to it. The kernel and its dependencies will then be compiled, and a bootable ISO, `jOSh.iso` will be generated. If you have qemu installed, you can boot this ISO in qemu with
```
make test
```
