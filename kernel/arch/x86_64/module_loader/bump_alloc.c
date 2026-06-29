#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <kernel/tty.h>
#include "bump_alloc.h"

typedef struct {
  size_t addr;
  bool init;
  bool locked;
} BumpAllocator;

static BumpAllocator alloc = {0, false};

static void bump_panic(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  term_set_fg(4);
  vprintf(fmt, args);
  __asm__ volatile (
                    ".panic.hang:\n\r"
                    "cli\n\r"
                    "hlt\n\r"
                    "jmp .panic.hang\n\r"
                    : : :
                    );
}

void bump_init(const size_t addr) {
  alloc.addr = addr;
  alloc.init = true;
  alloc.locked = false;
}

void bump_align(const size_t boundary) {
  if (!alloc.init) return;
  const size_t rem = alloc.addr % boundary;
  if (rem == 0) return;
  alloc.addr += boundary - rem;
}

void *bump_malloc(const size_t size) {
  if (!alloc.init) bump_panic("[!] Bump allocator used but not yet initialised!\n");
  if (alloc.locked && size != 0) bump_panic("[!] Bump allocator used after locking!\n");
  void* p = (void*)alloc.addr;
  alloc.addr += size;
  return p;
}

void bump_lock() {
  alloc.locked = true;
}
