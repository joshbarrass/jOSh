#ifndef __MEMORY_TYPES_H
#define __MEMORY_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef size_t page_number_t;
typedef uintptr_t phys_addr_t;
typedef void* virt_addr_t;

typedef struct {
  phys_addr_t offset;
  size_t n_pages;
} phys_region_t;

#endif
