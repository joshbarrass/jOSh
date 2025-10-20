#include <kernel/memory/pmm.h>
#include <stddef.h>
#include <stdint.h>
#include <kernel/mmap.h>
#include <kernel/memory/constants.h>
#include <kernel/panic.h>

// Statically allocate a bitmap that covers us up to the 4GB
// boundary. This will be pretty small, even on systems that don't
// have much memory, and will allow us to set up the memory manager
// with more than enough to spare. Once the memory manager is up and
// running, we can dynamically allocate a second bitmap for the memory
// above that, and stitch the two together behind the scenes.
#define PMM_4GB_CONSTANT (4ULL*1024ULL*1024ULL*1024ULL)
#define PMM_4GB_BITMAP_LENGTH (PMM_4GB_CONSTANT / PAGE_SIZE / PMM_STATES_PER_ENTRY)
static PMMEntry pmm_bitmap_4GB[PMM_4GB_BITMAP_LENGTH];

// Keep track of the lowest recently freed page (LRFP). If we go to do
// an allocation, we use this as a starting point for finding free
// pages. If we want one page with no constraints, we can return it
// immediately (if it's free). If we need a run of pages, or we need
// pages with certain constraints, we can use this is a starting point
// to find a valid page.
//
// It should never be assumed that the page this points to is actually
// free. You should always check the page is free before using it, and
// be prepared to scan for a free page if it isn't.
static const void* current_LRFP = NULL;

static inline size_t page_addr_to_ID(const void *const addr) {
  return ((const uintptr_t)addr & MEMORY_ADDRESS_MASK) / PAGE_SIZE;
}

static PageState pmm_get_page_state_by_ID(const size_t i) {
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

static PageState pmm_get_page_state(const void *addr) {
  const size_t i = page_addr_to_ID(addr);
  return pmm_get_page_state_by_ID(i);
}

static void pmm_set_page_state_by_ID(const size_t i, const PageState state) {
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

static void pmm_set_page_state(const void *addr, const PageState state) {
  const size_t i = page_addr_to_ID(addr);
  return pmm_set_page_state_by_ID(i, state);
}

#include <stdio.h>

static size_t count_free_pages(PMMEntry *bitmap, const size_t n) {
  const uint64_t *b = (const uint64_t*)bitmap;
  const size_t length = n * sizeof(PMMEntry) / sizeof(uint64_t);
  size_t free_pages = 0;
  for (size_t i = 0; i < length; ++i) {
    free_pages += __builtin_popcountll(b[i]);
  }
  return free_pages;
}

static void initialise_bitmap(PMMEntry *bitmap, const size_t n) {
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
  for (void *addr = NULL; addr < first_free_page; addr += PAGE_SIZE) {
    pmm_set_page_state(addr, PAGE_USED);
  }
  printf("    [*] Low memory reserved\n");

  // start the lowest free page tracker
  current_LRFP = first_free_page;

  const size_t free_pages = count_free_pages(pmm_bitmap_4GB, PMM_4GB_BITMAP_LENGTH);
  printf("    [*] %zu free pages (%zuKiB)\n", free_pages, 4*free_pages);
}

void pmm_free_pages(const void *addr, const size_t count) {
  // update the lowest free page pointer if this page is lower than
  // the current value
  current_LRFP = (addr < current_LRFP) ? addr : current_LRFP;
  const size_t id = page_addr_to_ID(addr);
  for (size_t i = 0; i < count; ++i) {
    pmm_set_page_state_by_ID(id + i, PAGE_FREE);
  }
}

// this is intended as a fast algorithm that isn't picky about the
// page it allocates, intended for quickly getting a single page
// without contraints
#if PAGE_STATE_WIDTH == 1
static const size_t find_one_free_page_ID() {
  if (pmm_get_page_state(current_LRFP) == PAGE_FREE) {
    const void *to_return = current_LRFP;
    return (uintptr_t)to_return / PAGE_SIZE;
  }

  // If we're here, we need to scan for the next free page.
  //
  // Order of bitfields is determined by the ABI [1]. x86-64 uses
  // System-V, which states that bit-fields are allocated from right
  // to left [2][3], i.e., LSB to MSB. This means if we cast the
  // bitmap to uint64_t, we can test 64 pages at a time for a single
  // free page in that range. Then we can use __builtin_ctzll to get
  // the number of trailing zeroes, which tells us the first free page
  // in that 64-page range. This lets us scan for a free page very
  // quickly. This only works if PAGE_STATE_WIDTH = 1.
  //
  // [1] https://gcc.gnu.org/onlinedocs/gcc/Structures-unions-enumerations-and-bit-fields-implementation.html
  // [2] https://stackoverflow.com/a/73780500
  // [3] https://cs61.seas.harvard.edu/site/pdf/x86-64-abi-20210928.pdf
  const uint64_t *b = (const uint64_t*)pmm_bitmap_4GB;
  const size_t page_number = ((uintptr_t)current_LRFP & MEMORY_ADDRESS_MASK) / PAGE_SIZE;
  printf("Starting from page number %zu", page_number);
  const size_t page_index = page_number / PMM_STATES_PER_ENTRY;
  printf(" = bitmap index %zu", page_index);
  const size_t start_index = page_index / (sizeof(uint64_t) / sizeof(PMMEntry));
  printf(" = uint64 index %zu\n", start_index);
  for (size_t i = start_index; i < sizeof(pmm_bitmap_4GB) / sizeof(uint64_t);
       ++i) {
    if (b[i] == 0) continue; // no free pages in this block of 64
    const int leading_zeroes = __builtin_ctzll(b[i]);

    printf("Found uint64 index %zu with %d trailing zeroes\n", i, leading_zeroes);

    const size_t bitmap_index = i * sizeof(uint64_t) / sizeof(PMMEntry);
    const size_t page_number_to_return = bitmap_index * PMM_STATES_PER_ENTRY + leading_zeroes;
    printf(" = page number %zu\n", page_number_to_return);
    return page_number_to_return;
  }

  // if we can't find a free page, we're (currently) screwed
  kpanic("Failed to find a free page!");
  return 0;
}
#endif

static inline const size_t find_free_page_IDs(const size_t count) {
  if (count > 1) {
    kpanic("Multiple page allocation not implemented!");
  }
  return find_one_free_page_ID();
}

const void *pmm_get_pages(const size_t count) {
  const size_t id = find_free_page_IDs(count);
  const void* addr = (const void*)(PAGE_SIZE * id);
  for (size_t i = 0; i < count; ++i) {
    pmm_set_page_state_by_ID(id + i, PAGE_USED);
  }
  return addr;
}
