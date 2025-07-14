#include <multiboot.h>

extern const MIS *mis;
extern void kernel_main();

volatile static char stack[16*1024] __attribute__((section(".bss"))); // 16kB stack

__attribute__((naked)) void _entry() {
  __asm__ __volatile__ (
    // set up the stack
    "lea stack+16*1024, %rsp\r\n"
    // forward EAX and EBX according to sysv convention
    // arg1 passed as rdi
    // arg2 passed as rsi
    "xor %rdi, %rdi\r\n"
    "mov %eax, %edi\r\n"
    "xor %rsi, %rsi\r\n"
    "mov %ebx, %esi\r\n"
    "jmp _entry_c\r\n"
                        );
}
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
