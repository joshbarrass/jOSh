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
#include "module_loader/multiboot.h"

static char *stack[16*1024] __attribute__((section(".bss"))); // 16-bit stack

static const char *welcomeMessage = "Welcome to jOSh! "
#ifdef ARCH_64
  "(64-bit)";
#elif ARCH_32
  "(32-bit)";
#else
;
#endif

const MIS *mis = NULL;

void kernel_main();

__attribute__((optimize("O0"))) void _entry() {
  // bind our registers to variable names to ensure they don't get
  // ruined
  register volatile uint32_t rax asm("eax");
  register volatile MIS *rbx asm("ebx");

  // set up a new stack, since the loader stack may not be sufficient
  // (and if it is, we've probably clobbered it loading the kernel ELF
  // anyway), then call the kernel
  asm volatile (
       #ifdef ARCH_64
       "mov %0, %%rsp\r\n"
       #elif ARCH_32
       "mov %0, %%esp\r\n"
       #endif
       :
       : "r"(stack+sizeof(stack)/sizeof(stack[0])-1)
#ifdef ARCH_64
       : "rax", "rbx"
#elif ARCH_32
       : "eax", "ebx"
#endif
       );

  // if we were booted by a multiboot, we can save the pointer to the
  // MIS
  if ((uint32_t)rax == (uint32_t)0x2BADB002) {
    mis = (MIS*)rbx;
  }
  kernel_main();

  // create a footer to hang if the kernel ever returns here
  asm volatile (
                ".hang:\r\n"
                "cli\r\n"
                "hlt\r\n"
                "jmp .hang\r\n"
                );
}

void kernel_main() {
  clear_screen();
  print_string(welcomeMessage, 0, 0);
  if (mis != NULL) {
    print_string(get_mod_string(&get_mods(mis)[0]), 0, 1);
  }

  return;
}
