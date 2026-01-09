#ifndef __X86_64_PAGING_H
#define __X86_64_PAGING_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <kernel/memory/constants.h>
#include <kernel/memory/types.h>

#define ENTRIES_PER_PAGE_TABLE (511)

typedef unsigned short ptindex_t;

/*
  Order of bitfields is determined by the ABI [1]. x86-64 uses
  System-V, which states that bit-fields are allocated from right
  to left [2][3], i.e., LSB to MSB.
*/

typedef struct __attribute__((packed)) {
  bool present : 1;
  bool writeable : 1;
  bool user : 1;
  bool write_through : 1;
  bool disable_cache : 1;
  bool accessed : 1;
  bool dirty : 1;
  bool page_size : 1;
  bool global : 1;
  uint8_t user_defined_l : 3;
  uintptr_t addr_shr_12 : (MEMORY_ADDRESS_BITS - 12);
#if (MEMORY_ADDRESS_BITS < 52)
  uint64_t __reserved_1 : (52 - MEMORY_ADDRESS_BITS);
#endif
  uint8_t user_defined_h : 7;
  uint8_t protection_key : 4;
  bool execute_disable : 1;
} PageTableEntry;

__attribute__((always_inline)) static inline phys_addr_t PTE_get_addr(const PageTableEntry pte) {
  return pte.addr_shr_12 << 12;
}

__attribute__((always_inline)) static inline void PTE_set_addr(PageTableEntry *pte, const phys_addr_t addr) {
  pte->addr_shr_12 = addr >> 12;
}

struct ptindices {
  ptindex_t pml4t_i, pdpt_i, pd_i, pt_i;
};

struct ptindices virt_addr_to_ptindices(const virt_addr_t addr);

bool create_page_table_entry(const virt_addr_t virt_addr, const PageTableEntry entry);

void invlpg(virt_addr_t addr);

#endif
