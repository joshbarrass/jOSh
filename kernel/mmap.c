#include <kernel/mmap.h>
#include <multiboot.h>

static const char * const MMAP_STRING_FREE = "FREE";
static const char * const MMAP_STRING_RSVD = "RSVD";
static const char * const MMAP_STRING_ACPI = "ACPI";
static const char * const MMAP_STRING_HIBERNATE = "RESH";
static const char * const MMAP_STRING_BAD = "BAD ";

const char *const get_mmap_type_string(uint32_t type) {
  switch (type) {
  case MULTIBOOT_MMAP_TYPE_FREE:
    return MMAP_STRING_FREE;
  case MULTIBOOT_MMAP_TYPE_RESERVED:
    return MMAP_STRING_RSVD;
  case MULTIBOOT_MMAP_TYPE_ACPI:
    return MMAP_STRING_ACPI;
  case MULTIBOOT_MMAP_TYPE_HIBERNATION:
    return MMAP_STRING_HIBERNATE;
  case MULTIBOOT_MMAP_TYPE_BAD:
    return MMAP_STRING_BAD;
  default:
    return MMAP_STRING_RSVD;
  }
}

mmap_iterator new_mmap_iterator(const void *mmap, const uint32_t length) {
  mmap_iterator iter = { mmap, length, 0 };
  return iter;
}

mmap *mmap_iterator_next(mmap_iterator *iter) {
  if (iter->offset >= iter->length) return 0;
  mmap *entry = (mmap*)(iter->mmap + iter->offset);
  iter->offset += entry->size + sizeof(entry->size);
  return entry;
}
