#include <kernel/x86_64/control_registers.h>

uint64_t read_CR3() {
  uint64_t cr3;
  asm volatile (
                "mov %%cr3, %0\r\n"
                : "=r" (cr3)
                );
  return cr3;
}
