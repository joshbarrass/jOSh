#ifndef __BOOTSTRAP_STATIC_BUMP_H
#define __BOOTSTRAP_STATIC_BUMP_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
  size_t size;
  size_t v;
  void* buf;
} StaticBumper;

#define STATIC_BUMP_ALLOCATOR(BUF)                                             \
  { sizeof(BUF), 0, (BUF) }

#define SB_ALLOC_ALIGNED(BUMPER, T)  ( ( T * )sb_aligned_alloc(BUMPER, sizeof( T ), _Alignof( T )) )

void *sb_alloc(StaticBumper *bumper, const size_t size);
void sb_align(StaticBumper *bumper, const size_t boundary);
void *sb_aligned_alloc(StaticBumper *bumper, const size_t size,
                       const size_t boundary);

#endif
