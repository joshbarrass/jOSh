#ifndef __MEMORY_CONSTANTS_H
#define __MEMORY_CONSTANTS_H 1

#include <stdint.h>

#define PAGE_SIZE (4096ULL)
#define MEMORY_ADDRESS_BITS (48ULL)

static const uintptr_t MEMORY_ADDRESS_MASK = (1ULL << MEMORY_ADDRESS_BITS) - 1;

#define USER_VADDR_START (2*1024*1024)
#define KERNEL_VADDR_START (0xfffffe8000200000)

#endif
