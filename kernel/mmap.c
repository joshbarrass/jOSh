#include <kernel/mmap.h>
#include <multiboot2.h>

static const char * const MMAP_STRING_FREE = "FREE";
static const char * const MMAP_STRING_RSVD = "RSVD";
static const char * const MMAP_STRING_ACPI = "ACPI";
static const char * const MMAP_STRING_HIBERNATE = "RESH";
static const char * const MMAP_STRING_BAD = "BAD ";

const char *const get_mmap_type_string(uint32_t type) {
  switch (type) {
  case MULTIBOOT2_MMAP_TYPE_FREE:
    return MMAP_STRING_FREE;
  case MULTIBOOT2_MMAP_TYPE_ACPI:
    return MMAP_STRING_ACPI;
  case MULTIBOOT2_MMAP_TYPE_HIBERNATION:
    return MMAP_STRING_HIBERNATE;
  case MULTIBOOT2_MMAP_TYPE_BAD:
    return MMAP_STRING_BAD;
  default:
    return MMAP_STRING_RSVD;
  }
}

mmap_iterator new_mmap_iterator(const m2is_mmap *mmap) {
  mmap_iterator iter = {
    .first_entry = mmap->entries,
    .length = mmap->tag.size - sizeof(uint32_t)*2,
    .entry_size = mmap->entry_size,
    .offset = 0,
  };
  return iter;
}

mmap_entry *mmap_iterator_next(mmap_iterator *iter) {
  if (iter->offset >= iter->length) return NULL;
  mmap_entry *entry = (mmap_entry*)(iter->first_entry + iter->offset);
  iter->offset += iter->entry_size;
  return entry;
}
