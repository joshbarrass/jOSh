#include <kernel/memory/constants.h>
#define PAGING_CONSTANTS_ONLY
#include <kernel/x86_64/memory/paging.h>
#include "kmap.h"
#include "paging.h"

static const uint64_t KSPACE_END = KERNEL_VADDR_START + (ENTRIES_PER_PAGE_TABLE * ENTRIES_PER_PAGE_TABLE * ENTRIES_PER_PAGE_TABLE * PAGE_SIZE);

static inline uint64_t round_up_addr(const uint64_t addr) {
  const uint64_t remainder = addr % PAGE_SIZE;
  if (remainder == 0) return addr;
  return addr + (PAGE_SIZE - remainder);
}

static inline uint64_t round_down_addr(const uint64_t addr) {
  const uint64_t remainder = addr % PAGE_SIZE;
  return addr - remainder;
}

static inline size_t pages_needed_for_size(const size_t size) {
  const size_t pages = size / PAGE_SIZE;
  const size_t remainder = size % PAGE_SIZE;
  if (remainder == 0) return pages;
  return pages + 1;
}

static inline bool is_virt_addr_used(const uint64_t * const pml4t, const uint64_t virt_addr) {
  return get_page_table(pml4t, virt_addr, NULL) != NULL;
}

uint64_t kmap(uint64_t * const pml4t, const uint64_t phys_addr, const size_t size) {
  uint64_t vaddr = KERNEL_VADDR_START;
  vaddr = round_up_addr(vaddr);

  // get the range of the physical pages we need to use
  const uint64_t phys_page_end = round_up_addr(phys_addr + size);
  const uint64_t phys_page = round_down_addr(phys_addr);
  const uint64_t phys_offset = phys_addr - phys_page;
  const size_t pages_required = pages_needed_for_size(phys_page_end - phys_page);

  size_t found_free_pages = 0;
  uint64_t start_addr = vaddr;

  while (found_free_pages < pages_required && vaddr < KSPACE_END) {
    if (is_virt_addr_used(pml4t, vaddr)) {
      // reset
      vaddr += PAGE_SIZE;
      found_free_pages = 0;
      start_addr = vaddr;
      continue;
    }
    ++found_free_pages;
    vaddr += PAGE_SIZE;
  }

  if (found_free_pages < pages_required) return 0;

  // create the mapping
  for (size_t i = 0; i < pages_required; ++i) {
    const uint64_t addr = start_addr + i*PAGE_SIZE;
    const uint64_t page_addr = phys_page + i*PAGE_SIZE;
    uint64_t *pte = get_or_create_page_table_entry(pml4t, addr);
    *pte = page_addr | 3;
  }

  return start_addr + phys_offset;
}
