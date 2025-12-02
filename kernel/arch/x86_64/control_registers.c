#include <kernel/x86_64/control_registers.h>

uint64_t read_CR0() {
  uint64_t cr0;
  asm volatile (
                "mov %%cr0, %0\r\n"
                : "=r" (cr0)
                );
  return cr0;
}

uint64_t read_CR2() {
  uint64_t cr2;
  asm volatile (
                "mov %%cr2, %0\r\n"
                : "=r" (cr2)
                );
  return cr2;
}

uint64_t read_CR3() {
  uint64_t cr3;
  asm volatile (
                "mov %%cr3, %0\r\n"
                : "=r" (cr3)
                );
  return cr3;
}

uint64_t read_CR4() {
  uint64_t cr4;
  asm volatile (
                "mov %%cr4, %0\r\n"
                : "=r" (cr4)
                );
  return cr4;
}
