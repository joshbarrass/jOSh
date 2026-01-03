#ifndef __MEMORY_PMM_H
#define __MEMORY_PMM_H 1

#include <multiboot.h>
#include <kernel/memory/types.h>

// width of a page state in *bits*
#define PAGE_STATE_WIDTH (1ULL)
typedef enum {
  PAGE_USED = 0,
  PAGE_FREE = 1
} PageState;

typedef struct __attribute__((packed)) {
  PageState _0 : PAGE_STATE_WIDTH;
  PageState _1 : PAGE_STATE_WIDTH;
  PageState _2 : PAGE_STATE_WIDTH;
  PageState _3 : PAGE_STATE_WIDTH;
  PageState _4 : PAGE_STATE_WIDTH;
  PageState _5 : PAGE_STATE_WIDTH;
  PageState _6 : PAGE_STATE_WIDTH;
  PageState _7 : PAGE_STATE_WIDTH;
} PMMEntry;

#define PMM_STATES_PER_ENTRY (sizeof(PMMEntry)*8ULL/PAGE_STATE_WIDTH)

void pmm_init(const phys_addr_t first_free_page, const mmap *memory_map, const uint32_t mmap_length);
void pmm_free_pages(const phys_addr_t addr, const size_t count);
const phys_addr_t pmm_alloc_pages(const size_t count);

#endif
