#include <kernel/bootstrap/static_bump.h>

void sb_align(StaticBumper *bumper, const size_t boundary) {
  // Align relative to the underlying memory address, not relative to
  // the start of the buffer. This should allow arbitary alignment of
  // the buffer (though you should probably align it anyway) without
  // breaking anything.
  const uintptr_t addr = (uintptr_t)(&bumper->buf[bumper->v]);
  const size_t rem = addr % boundary;
  if (rem == 0) return;
  bumper->v += boundary - rem;
}

void *sb_alloc(StaticBumper *bumper, const size_t size) {
  if (bumper->v + size > bumper->size) {
    return NULL;
  }
  void *ptr = &bumper->buf[bumper->v];
  bumper->v += size;
  return ptr;
}

void *sb_aligned_alloc(StaticBumper *bumper, const size_t size,
                       const size_t boundary) {
  const size_t oldpos = bumper->v;
  sb_align(bumper, boundary);
  void * const addr = sb_alloc(bumper, size);
  if (addr == NULL) {
    // allocation failed due to not enough size; reset the position to
    // pre-alignment
    bumper->v = oldpos;
    return NULL;
  }
  return addr;
}
