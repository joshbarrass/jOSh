#ifndef __MMAP_H
#define __MMAP_H

#include <stdint.h>
#include <stddef.h>
#include <multiboot2.h>

typedef struct {
  const void *first_entry;
  const uint32_t length;
  const uint32_t entry_size;
  size_t offset;
} mmap_iterator;

const char *const get_mmap_type_string(uint32_t type);
mmap_iterator new_mmap_iterator(const m2is_mmap *mmap);
mmap_entry *mmap_iterator_next(mmap_iterator *iter);

#endif
