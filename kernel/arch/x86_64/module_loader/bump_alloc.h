#ifndef __BUMP_ALLOC_H
#define __BUMP_ALLOC_H

#include <stddef.h>

void bump_init(const size_t addr);
void* bump_malloc(const size_t size);
void bump_align(const size_t boundary);

#endif
