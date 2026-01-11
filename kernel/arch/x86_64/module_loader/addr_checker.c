#include <multiboot2.h>
#include <stdint.h>
#include "addr_checker.h"

// One advantage of the multiboot2 information struct is that every
// tag is completely self-contained; it should not reference any
// memory outside of itself. The maximum address is therefore just the
// address of the MIS + its total size
uint64_t get_M2IS_max_addr(const M2IS *mis) {
  return (uint64_t)(uintptr_t)((char*)mis + mis->size);
}
