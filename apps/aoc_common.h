#ifndef AOC_COMMON_H
#define AOC_COMMON_H

#include <kernel/memory/constants.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>

char *get_input();

#define array_malloc(T) static T *malloc_##T##_array (const size_t n) { \
  const size_t bytes = sizeof(T) * n; \
  size_t pages_needed = bytes / PAGE_SIZE; \
  const size_t remainder = bytes % PAGE_SIZE; \
  if (remainder != 0) ++pages_needed; \
  const uintptr_t phys_page = (uintptr_t)pmm_alloc_pages(pages_needed); \
  return (T*)vmm_kmap(phys_page, bytes, 0, 0); \
}

#define array_malloc_verbose(T) static T *malloc_##T##_array (const size_t n) { \
  const size_t bytes = sizeof(T) * n; \
  size_t pages_needed = bytes / PAGE_SIZE; \
  printf("Need %zu pages to allocate " #T "[%zu]\n", pages_needed, n);     \
  const size_t remainder = bytes % PAGE_SIZE; \
  if (remainder != 0) ++pages_needed; \
  const uintptr_t phys_page = (uintptr_t)pmm_alloc_pages(pages_needed); \
  return (T*)vmm_kmap(phys_page, bytes, 0, 0); \
}

#endif
