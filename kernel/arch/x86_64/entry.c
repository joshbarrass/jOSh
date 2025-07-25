#include <multiboot.h>

extern const MIS *mis;
extern void kernel_main();

__attribute__((noreturn,sysv_abi)) void _entry_c(uint32_t rax, MIS *rbx) {
  // if we were booted by a multiboot, we can save the pointer to the
  // MIS
  if ((uint32_t)rax == (uint32_t)0x2BADB002) {
    mis = (MIS*)rbx;
  }
  kernel_main();

  // create a footer to hang if the kernel ever returns here
  while (1) {
    asm volatile(".hang:\r\n"
                 "cli\r\n"
                 "hlt\r\n");
  }
}
