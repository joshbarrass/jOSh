#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_64 1
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#define ARCH_32 1
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tty.h"

static char *stack[16*1024] __attribute__((section(".bss"))); // 16-bit stack

static const char *welcomeMessage = "Welcome to jOSh! "
#ifdef ARCH_64
  "(64-bit)";
#elif ARCH_32
  "(32-bit)";
#else
;
#endif

void _entry() {
  // set up a new stack, since the loader stack may not be sufficient
  // (and if it is, we've probably clobbered it loading the kernel ELF
  // anyway), then call the kernel
  asm (
       #ifdef ARCH_64
       "mov %0, %%rsp\r\n"
       #elif ARCH_32
       "mov %0, %%esp\r\n"
       #endif
       "call kernel_main\r\n"
       ".hang:\r\n"
       "cli\r\n"
       "hlt\r\n"
       "jmp .hang\r\n"
       :
       : "r"(stack)
       :
       );
}

void kernel_main() {
  clear_screen();
  screen->character = 'P';
  print_string(welcomeMessage, 0, 0);
  
  return;
}
