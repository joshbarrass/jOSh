#ifndef __INTERRUPTS_X86_64_H
#define __INTERRUPTS_X86_64_H

#include <stdint.h>

typedef struct __attribute__((packed)) {
  uint16_t offset_0;
  uint16_t segment;
  uint8_t ist : 3;
  uint8_t __reserved_0 : 5;
  uint8_t attrs;
  uint16_t offset_1;
  uint32_t offset_2;
  uint32_t __reserved_1;
} GateDescriptor64;

#endif
