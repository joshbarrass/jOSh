#ifndef __MEMORY_DYNAMIC_H
#define __MEMORY_DYNAMIC_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
  FLAG_NONE = 0
} malloc_flags_t;

typedef enum {
  MALLOC_TYPE_RSVD = 0,
  MALLOC_TYPE_FULLPAGE = 1
} malloc_type_t;

typedef struct __attribute__((aligned(__BIGGEST_ALIGNMENT__))) {
  malloc_type_t type;
} malloc_header_t;

void *kmalloc(const size_t size, const malloc_flags_t flags);
void *kcalloc(const size_t size, const malloc_flags_t flags);

#endif
