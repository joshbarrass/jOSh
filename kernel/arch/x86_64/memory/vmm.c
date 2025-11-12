#include <kernel/memory/vmm.h>

enum PageTableLevel {
  PML4T = 3,
  PDPT = 2,
  PD = 1,
  PT = 0
};
typedef enum PageTableLevel PageTableLevel;

typedef uint64_t PageTableEntry;

typedef unsigned short ptindex_t;

inline __attribute__((always_inline)) uintptr_t canonicalise_addr(uintptr_t addr) {
  return ((int64_t)addr << 16) >> 16;
}

// returns &CR3[i][j][k][l] if level == PT
// returns &CR3[i][j][k]    if level == PD
// returns &CR3[i][j]       if level == PDPT
// returns &CR3[i]          if level == PML4T
PageTableEntry *get_page_table_pointer(const PageTableLevel level, const ptindex_t i, const ptindex_t j, const ptindex_t k, const ptindex_t l) {
  ptindex_t indices[4] = {l, k, j, i};
  
  uintptr_t base_addr = 0;

  for (int i = 0; i < 4; ++i) {
    if (i <= level) {
      base_addr |= 511;
    } else {
      base_addr |= (indices[i] & 511);
    }
    base_addr <<= 9ULL;
  }
  base_addr <<= 3ULL;

  return (PageTableEntry*)canonicalise_addr(base_addr + 8*indices[level]);
}

// Returns the virtual address corresponding to (*PageTableEntry)CR3
// This can be treated as a PageTableEntry[512]
PageTableEntry *get_PML4T() {
  return get_page_table_pointer(PML4T, 0, 0, 0, 0);
}

// Returns the virtual address corresponding to (**PageTableEntry)CR3[PML4T_i]
// This can be treated as a PageTableEntry[512]
PageTableEntry *get_PDPT(const ptindex_t PML4T_i) {
  return get_page_table_pointer(PDPT, PML4T_i, 0, 0, 0);
}

// Returns the virtual address corresponding to (***PageTableEntry)CR3[PML4T_i][PDPT_i]
// This can be treated as a PageTableEntry[512]
PageTableEntry *get_PD(const ptindex_t PML4T_i, const ptindex_t PDPT_i) {
  return get_page_table_pointer(PD, PML4T_i, PDPT_i, 0, 0);
}

// Returns the virtual address corresponding to (****PageTableEntry)CR3[PML4T_i][PDPT_i][PD_i]
// This can be treated as a PageTableEntry[512]
PageTableEntry *get_PT(const ptindex_t PML4T_i, const ptindex_t PDPT_i, const ptindex_t PD_i) {
  return get_page_table_pointer(PT, PML4T_i, PDPT_i, PD_i, 0);
}
