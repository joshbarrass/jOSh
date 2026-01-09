#include <kernel/memory/types.h>
#include <kernel/memory/pmm.h>
#include <kernel/x86_64/memory/paging.h>
#include <kernel/x86_64/memory/recursive_pt.h>

#ifdef VERBOSE_PAGING
#include <stdio.h>
#endif

struct ptindices virt_addr_to_ptindices(const virt_addr_t a) {
  struct ptindices out;
  uintptr_t addr = (uintptr_t)a;
  
  addr >>= 12;
  out.pt_i = addr & 511;
  addr >>= 9;
  out.pd_i = addr & 511;
  addr >>= 9;
  out.pdpt_i = addr & 511;
  addr >>= 9;
  out.pml4t_i = addr & 511;

  return out;
}

static void clear_pagetableentry(PageTableEntry *entry) {
  entry->present = 0;
  entry->writeable = 0;
  entry->user = 0;
  entry->write_through = 0;
  entry->disable_cache = 0;
  entry->accessed = 0;
  entry->dirty = 0;
  entry->page_size = 0;
  entry->global = 0;
  entry->user_defined_l = 0;
  entry->addr_shr_12 = 0;
  #if (MEMORY_ADDRESS_BITS < 52)
  entry->__reserved_1 = 0;
  #endif
  entry->user_defined_h = 0;
  entry->protection_key = 0;
  entry->execute_disable = 0;
}

static void clear_pagetable(PageTableEntry *table) {
  for (size_t i = 0; i < ENTRIES_PER_PAGE_TABLE; ++i) {
    clear_pagetableentry(table + i);
  }
}

void invlpg(const virt_addr_t addr) {
  __asm__ volatile (
                    "invlpg (%0)"
                    : : "r" (addr)
                    );
  #ifdef VERBOSE_PAGING
  printf("Invalidated %#18zx\n", (uintptr_t)addr);
  #endif
}

static void create_intermediate_entry(PageTableEntry *target,
                                      PageTableEntry *recursive_addr, const bool user) {
  const uintptr_t phys_page = (uintptr_t)pmm_alloc_pages(1);
  clear_pagetableentry(target);
  target->addr_shr_12 = phys_page >> 12;
  target->writeable = true;
  target->user = user;
  target->present = true;
  // need to invalidate the recursive entry so we can manage this new
  // table correctly
  invlpg(recursive_addr);
  clear_pagetable(recursive_addr);
}

// create_page_table_entry will create all of the necessary page table
// structures to create a given page table entry for a given virtual
// address. The upper levels of the page table hierarchy will be
// created automatically with sane defaults if they do not exist. This
// generally means upper tables:
//   1) are marked present
//   2) are marked writeable (to give maximum fine control at lower levels)
//   3) inherit their user/supervisor bit from the input entry
//   4) have XD set to 0 (to give maximum fine control)
//   5) have all other bits set to 0
// If a new level (PageTableEntry[512]) must be created, the physical
// memory will be taken directly from the PMM, and will be configured
// via the recursive entry.
bool create_page_table_entry(const virt_addr_t virt_addr, const PageTableEntry entry) {
  const struct ptindices is = virt_addr_to_ptindices(virt_addr);
  PageTableEntry *pml4t = get_PML4T();
  if (!pml4t[is.pml4t_i].present) { // PDPT is missing
    create_intermediate_entry(&pml4t[is.pml4t_i], get_PDPT(is.pml4t_i), entry.user);
    #ifdef VERBOSE_PAGING
    printf("Created PDPT @ PML4T[%u]\n", is.pml4t_i);
    #endif
  }
  PageTableEntry *pdpt = get_PDPT(is.pml4t_i);
  if (!pdpt[is.pdpt_i].present) { // PD is missing
    create_intermediate_entry(&pdpt[is.pdpt_i], get_PD(is.pml4t_i, is.pdpt_i), entry.user);
    #ifdef VERBOSE_PAGING
    printf("Created PD @ PDPT[%u]\n", is.pdpt_i);
    #endif
  }
  PageTableEntry *pd = get_PD(is.pml4t_i, is.pdpt_i);
  if (!pd[is.pd_i].present) { // PT is missing
    create_intermediate_entry(&pd[is.pd_i], get_PT(is.pml4t_i, is.pdpt_i, is.pd_i), entry.user);
    #ifdef VERBOSE_PAGING
    printf("Created PT @ PD[%u]\n", is.pd_i);
    #endif
  }
  PageTableEntry *pt = get_PT(is.pml4t_i, is.pdpt_i, is.pd_i);
  pt[is.pt_i] = entry;
  invlpg(virt_addr);
  return true;
}
