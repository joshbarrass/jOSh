#include <kernel/memory/vmm.h>
#include <kernel/x86_64/memory/recursive_pt.h>

void vmm_init() {
  // We must fetch and store the physical address of the kernel-space
  // page table and recursive entry via the recursive entry. We will
  // need this to copy it into all new address spaces.

  // Kernel-space in x86_64 lives in a single PDPT (a single 512GB
  // block), so we need to save the address of this single PDPT.

  
}
