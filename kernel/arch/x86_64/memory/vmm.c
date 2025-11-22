#include <kernel/memory/vmm.h>
#include <kernel/x86_64/memory/recursive_pt.h>
#include <kernel/memory/constants.h>

static uintptr_t KSPACE_PDPT_PHYS_ADDR;

void vmm_init() {
  // We must fetch and store the physical address of the kernel-space
  // page table via the recursive entry. We will need this to copy it
  // into all new address spaces.

  // Kernel-space in x86_64 lives in a single PDPT (a single 512GB
  // block), so we need to save the address of this single PDPT.
  const PageTableEntry *PML4T = get_PML4T();
  // convert the kernel start address to an index
  const ptindex_t kspace_i = (KERNEL_VADDR_START & (511ULL << 39)) >> 39;
  KSPACE_PDPT_PHYS_ADDR = PTE_get_addr(PML4T[kspace_i]);
}
