#ifndef _KMAP_H
#define _KMAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

uint64_t kmap(uint64_t * const pml4t, uint64_t phys_addr, const size_t size);

#endif
