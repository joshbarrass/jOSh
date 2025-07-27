#include "paging.h"
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

