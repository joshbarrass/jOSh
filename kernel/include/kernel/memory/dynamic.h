#ifndef __MEMORY_DYNAMIC_H
#define __MEMORY_DYNAMIC_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
  FLAG_NONE = 0
} malloc_flags_t;

typedef struct __attribute__((aligned(__BIGGEST_ALIGNMENT__))) {
  
} malloc_header_t;

void *
kmalloc(const size_t size, const malloc_flags_t flags);

#endif
