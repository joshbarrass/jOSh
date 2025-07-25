#include <stdbool.h>
#include "bump_alloc.h"

typedef struct {
  size_t addr;
  bool init;
} BumpAllocator;

static BumpAllocator alloc = {0, false};

void bump_init(const size_t addr) {
  alloc.addr = addr;
  alloc.init = true;
}

void bump_align(const size_t boundary) {
  if (!alloc.init) return;
  const size_t rem = alloc.addr % boundary;
  if (rem == 0) return;
  alloc.addr += boundary - rem;
}

void *bump_malloc(const size_t size) {
  if (!alloc.init) return NULL;
  void* p = (void*)alloc.addr;
  alloc.addr += size;
  return p;
}
