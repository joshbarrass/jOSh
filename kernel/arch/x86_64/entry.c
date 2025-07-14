#include <multiboot.h>

extern const MIS *mis;
extern void kernel_main();

volatile static char stack[16*1024] __attribute__((section(".bss"))); // 16kB stack

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
