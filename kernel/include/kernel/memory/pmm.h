#ifndef __MEMORY_PMM_H
#define __MEMORY_PMM_H 1

#include <multiboot.h>

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

PageState pmm_get_page_state(const void *addr);
void pmm_set_page_state(const void *addr, const PageState state);
void initialise_pmm(const void *first_free_page, const mmap *memory_map, const uint32_t mmap_length);

#endif
