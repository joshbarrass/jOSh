#ifndef __X86_64_RECURSIVE_PT_H
#define __X86_64_RECURSIVE_PT_H 1

#include <stdint.h>
#include <kernel/x86_64/memory/paging.h>

#define RECURSIVE_ENTRY_INDEX 511

// Returns the virtual address corresponding to (*PageTableEntry)CR3
// This can be treated as a PageTableEntry[512]
PageTableEntry *get_PML4T();

// Returns the virtual address corresponding to (**PageTableEntry)CR3[PML4T_i]
// This can be treated as a PageTableEntry[512]
PageTableEntry *get_PDPT(const ptindex_t PML4T_i);

// Returns the virtual address corresponding to (***PageTableEntry)CR3[PML4T_i][PDPT_i]
// This can be treated as a PageTableEntry[512]
PageTableEntry *get_PD(const ptindex_t PML4T_i, const ptindex_t PDPT_i);

// Returns the virtual address corresponding to (****PageTableEntry)CR3[PML4T_i][PDPT_i][PD_i]
// This can be treated as a PageTableEntry[512]
PageTableEntry *get_PT(const ptindex_t PML4T_i, const ptindex_t PDPT_i, const ptindex_t PD_i);

#endif
