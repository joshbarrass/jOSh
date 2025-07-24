#ifndef __MMAP_H
#define __MMAP_H

#include <stdint.h>
#include <stddef.h>
#include <multiboot.h>

typedef struct {
  const void *mmap;
  const uint32_t length;
  size_t offset;
} mmap_iterator;

const char *const get_mmap_type_string(uint32_t type);
mmap_iterator new_mmap_iterator(const void *mmap, const uint32_t length);
mmap *mmap_iterator_next(mmap_iterator *iter);

#endif
