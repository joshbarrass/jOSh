#include <kernel/memory/pmm.h>
#include <stddef.h>
#include <stdint.h>
#include <kernel/mmap.h>
#include <kernel/memory/constants.h>

// Statically allocate a bitmap that covers us up to the 4GB
// boundary. This will be pretty small, even on systems that don't
// have much memory, and will allow us to set up the memory manager
// with more than enough to spare. Once the memory manager is up and
// running, we can dynamically allocate a second bitmap for the memory
// above that, and stitch the two together behind the scenes.
#define PMM_4GB_CONSTANT (4ULL*1024ULL*1024ULL*1024ULL)
#define PMM_4GB_BITMAP_LENGTH (PMM_4GB_CONSTANT / PAGE_SIZE / PMM_STATES_PER_ENTRY)
static PMMEntry pmm_bitmap_4GB[PMM_4GB_BITMAP_LENGTH];

PageState pmm_get_page_state(const void *addr) {
  const size_t i = ((uintptr_t)addr & MEMORY_ADDRESS_MASK) / PAGE_SIZE;
  const size_t index = i / PMM_STATES_PER_ENTRY; // TODO: safety checks?
  const size_t entry = i % PMM_STATES_PER_ENTRY;
  // TODO: once we add in a second bitmap for the memory above 4GB,
  // we'll need a switch to toggle between the two.
  const PMMEntry *bitmap = pmm_bitmap_4GB;
  PageState state;
  switch (entry) {
  case 0:
    state = bitmap[index]._0;
    break;
  case 1:
    state = bitmap[index]._1;
    break;
  case 2:
    state = bitmap[index]._2;
    break;
  case 3:
    state = bitmap[index]._3;
    break;
  case 4:
    state = bitmap[index]._4;
    break;
  case 5:
    state = bitmap[index]._5;
    break;
  case 6:
    state = bitmap[index]._6;
    break;
  case 7:
    state = bitmap[index]._7;
    break;
  }
  return state;
}

void pmm_set_page_state(const void *addr, const PageState state) {
  const size_t i = ((uintptr_t)addr & MEMORY_ADDRESS_MASK) / PAGE_SIZE;
  const size_t index = i / PMM_STATES_PER_ENTRY; // TODO: safety checks?
  const size_t entry = i % PMM_STATES_PER_ENTRY;
  // TODO: once we add in a second bitmap for the memory above 4GB,
  // we'll need a switch to toggle between the two.
  PMMEntry *bitmap = pmm_bitmap_4GB;
  switch (entry) {
  case 0:
    bitmap[index]._0 = state;
    break;
  case 1:
    bitmap[index]._1 = state;
    break;
  case 2:
    bitmap[index]._2 = state;
    break;
  case 3:
    bitmap[index]._3 = state;
    break;
  case 4:
    bitmap[index]._4 = state;
    break;
  case 5:
    bitmap[index]._5 = state;
    break;
  case 6:
    bitmap[index]._6 = state;
    break;
  case 7:
    bitmap[index]._7 = state;
    break;
  }
  return;
}

#include <stdio.h>

size_t count_free_pages(PMMEntry *bitmap, const size_t n) {
  const uint64_t *b = (const uint64_t*)bitmap;
  const size_t length = n * sizeof(PMMEntry) / sizeof(uint64_t);
  size_t free_pages = 0;
  for (size_t i = 0; i < length; ++i) {
    free_pages += __builtin_popcountll(b[i]);
  }
  return free_pages;
}

void initialise_bitmap(PMMEntry *bitmap, const size_t n) {
  uint64_t *b = (uint64_t*)pmm_bitmap_4GB;
  const size_t length = n * sizeof(PMMEntry) / sizeof(uint64_t);
  for (size_t i = 0; i < length; ++i) {
    b[i] = 0;
  }
}

void initialise_pmm(const void *first_free_page, const mmap *memory_map, const uint32_t mmap_length) {
  printf("[*] Initialising PMM...\n");

  // zero-fill the bitmap
  initialise_bitmap(pmm_bitmap_4GB, PMM_4GB_BITMAP_LENGTH);

  // start filling in the bitmap from the mmap
  mmap_iterator iter = new_mmap_iterator(memory_map, mmap_length);
  mmap *entry = mmap_iterator_next(&iter);
  while (entry != NULL) {
    PageState state;
    if (entry->type == MULTIBOOT_MMAP_TYPE_FREE) {
      state = PAGE_FREE;
    } else {
      state = PAGE_USED;
    }

    // loop through every page in this entry and set it
    for (void *addr = (void *)entry->base_addr;
         addr < (void *)(entry->base_addr + entry->length) &&
         addr < (void *)(PMM_4GB_CONSTANT);
         addr += PAGE_SIZE) {
      pmm_set_page_state(addr, state);
    }
    entry = mmap_iterator_next(&iter);
  }
  printf("    [*] Memory map loaded\n");

  // reserve any memory that's already in use
  for (void *addr = (void *)0; addr < first_free_page; addr += PAGE_SIZE) {
    pmm_set_page_state(addr, PAGE_USED);
  }
  printf("    [*] Low memory reserved\n");

  printf("    [*] 0x9F000 is ");
  if (pmm_get_page_state((void *)0x9F000) == PAGE_USED) {
    printf("USED\n");
  } else {
    printf("FREE\n");
  }

  printf("    [*] 0x7FDF000 is ");
  if (pmm_get_page_state((void *)0x7FDF000) == PAGE_USED) {
    printf("USED\n");
  } else {
    printf("FREE\n");
  }

  const size_t free_pages = count_free_pages(pmm_bitmap_4GB, PMM_4GB_BITMAP_LENGTH);
  printf("    [*] %zu free pages (%zuKiB)\n", free_pages, 4*free_pages);
}
