#ifndef __PAGING_H
#define __PAGING_H

#include <stdint.h>
#include <stddef.h>

#define LEN_PAGE_TABLE 512
#define PAGE_SIZE (sizeof(uint64_t)*LEN_PAGE_TABLE)
#define PAGE_ALIGNMENT 0x1000

void zero_page_table(uint64_t *table);

void virtual_to_page_table_indices(uint64_t addr, size_t *pml4t_i,
                                   size_t *pdpt_i, size_t *pd_i,
                                   size_t *pt_i);

uint64_t *fetch_page_table(uint64_t *table, const size_t i);

#endif
