#include "paging.h"
#include "bump_alloc.h"
#include <stdio.h>

void zero_page_table(uint64_t *table) {
  for (size_t i = 0; i < LEN_PAGE_TABLE; ++i) {
    table[i] = 0;
  }
}

int virtual_to_page_table_indices(uint64_t addr, size_t *pml4t_i,
                                   size_t *pdpt_i, size_t *pd_i,
                                   size_t *pt_i) {
  if (pml4t_i == NULL || pdpt_i == NULL || pd_i == NULL || pt_i == NULL) return -1;

  addr >>= 12;
  *pt_i = addr & 511;
  addr >>= 9;
  *pd_i = addr & 511;
  addr >>= 9;
  *pdpt_i = addr & 511;
  addr >>= 9;
  *pml4t_i = addr & 511;

  return 0;
}

uint64_t *fetch_page_table(uint64_t *table, const size_t i) {
  return (uint64_t*)(table[i] & (UINT64_MAX - 0xfff));
}

static uint64_t *bump_malloc_pt() {
  bump_align(PAGE_ALIGNMENT);
  uint64_t *addr = bump_malloc(PAGE_SIZE);
  return addr;
}

// Returns the address of the page table for the supplied virtual
// address under the supplied PML4T. If any of the intermediate
// structures (PDPT, PD, or the PT itself) do not exist, they are
// created with the bump allocator and added to the parent
// structure. If i is a non-null pointer, the index of the page table
// entry for the supplied virtual address will be stored in i.
uint64_t *get_or_create_page_table(uint64_t * const pml4t, const uint64_t virt_addr, size_t * const i) {
  // get the page table indices corresponding to the virtual address
  size_t i0, i1, i2, i3;
  if (virtual_to_page_table_indices(virt_addr, &i0, &i1, &i2, &i3) != 0) return NULL;
  if (i != NULL) *i = i3;

  // ensure all the tables exist
  uint64_t *pdpt = fetch_page_table(pml4t, i0);
  if (pdpt == NULL) {
    pml4t[i0] = (uint64_t)bump_malloc_pt() | 3;
    pdpt = fetch_page_table(pml4t, i0);
    zero_page_table(pdpt);
  }
  uint64_t *pd = fetch_page_table(pdpt, i1);
  if (pd == NULL) {
    pdpt[i1] = (uint64_t)bump_malloc_pt() | 3;
    pd = fetch_page_table(pdpt, i1);
    zero_page_table(pd);
  }
  uint64_t *pt = fetch_page_table(pd, i2);
  if (pt == NULL) {
    pd[i2] = (uint64_t)bump_malloc_pt() | 3;
    pt = fetch_page_table(pd, i2);
    zero_page_table(pt);
  }
  return pt;
}

// Returns the address of the page table entry for the supplied
// virtual address under the supplied PML4T. If any of the
// intermediate structures (PDPT, PD, or the PT itself) do not exist,
// they are created with the bump allocator and added to the parent
// structure. The returned address is for a specific entry in the PT,
// not the PT itself, and so should be used directly rather than
// indexed.
uint64_t *get_or_create_page_table_entry(uint64_t *const pml4t,
                                         const uint64_t virt_addr) {
  size_t i3;
  uint64_t *pt = get_or_create_page_table(pml4t, virt_addr, &i3);
  if (pt == NULL) return NULL;
  return &pt[i3];
}
