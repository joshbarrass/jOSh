#include <kernel/x86_64/memory/recursive_pt.h>

// x86-64 addresses are 48 bits, but are stored as sign-extended
// 64-bit values. This function performs the sign extension
// (canonicalisation) of a pointer.
static inline __attribute__((always_inline)) uintptr_t canonicalise_addr(uintptr_t addr) {
  return ((int64_t)addr << 16) >> 16;
}

// Returns the virtual address corresponding to (*PageTableEntry)CR3
// This can be treated as a PageTableEntry[512]
PageTableEntry *get_PML4T() {
  /* return get_page_table_pointer(PML4T, 0, 0, 0, 0); */
  return (PageTableEntry*)canonicalise_addr(((uintptr_t)RECURSIVE_ENTRY_INDEX << 39) | ((uintptr_t)RECURSIVE_ENTRY_INDEX << 30) | ((uintptr_t)RECURSIVE_ENTRY_INDEX << 21) | ((uintptr_t)RECURSIVE_ENTRY_INDEX << 12));
}

// Returns the virtual address corresponding to (**PageTableEntry)CR3[PML4T_i]
// This can be treated as a PageTableEntry[512]
PageTableEntry *get_PDPT(const ptindex_t PML4T_i) {
  return (PageTableEntry*)canonicalise_addr(((uintptr_t)RECURSIVE_ENTRY_INDEX << 39) | ((uintptr_t)RECURSIVE_ENTRY_INDEX << 30) | ((uintptr_t)RECURSIVE_ENTRY_INDEX << 21) | ((uintptr_t)PML4T_i << 12));
}

// Returns the virtual address corresponding to (***PageTableEntry)CR3[PML4T_i][PDPT_i]
// This can be treated as a PageTableEntry[512]
PageTableEntry *get_PD(const ptindex_t PML4T_i, const ptindex_t PDPT_i) {
  return (PageTableEntry*)canonicalise_addr(((uintptr_t)RECURSIVE_ENTRY_INDEX << 39) | ((uintptr_t)RECURSIVE_ENTRY_INDEX << 30) | ((uintptr_t)PML4T_i << 21) | ((uintptr_t)PDPT_i << 12));
}

// Returns the virtual address corresponding to (****PageTableEntry)CR3[PML4T_i][PDPT_i][PD_i]
// This can be treated as a PageTableEntry[512]
PageTableEntry *get_PT(const ptindex_t PML4T_i, const ptindex_t PDPT_i, const ptindex_t PD_i) {
  return (PageTableEntry*)canonicalise_addr(((uintptr_t)RECURSIVE_ENTRY_INDEX << 39) | ((uintptr_t)PML4T_i << 30) | ((uintptr_t)PDPT_i << 21) | ((uintptr_t)PD_i << 12));
}
