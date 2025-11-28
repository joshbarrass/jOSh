#include <kernel/memory/vmm.h>
#include <kernel/memory/constants.h>
#include <kernel/x86_64/memory/paging.h>
#include <kernel/x86_64/memory/recursive_pt.h>
#include <kernel/panic.h>

static const ptindex_t KSPACE_PML4T_INDEX = (KERNEL_VADDR_START & (511ULL << 39)) >> 39;
static const uintptr_t KSPACE_END = KERNEL_VADDR_START + (ENTRIES_PER_PAGE_TABLE * ENTRIES_PER_PAGE_TABLE * ENTRIES_PER_PAGE_TABLE * PAGE_SIZE);

static uintptr_t KSPACE_PDPT_PHYS_ADDR;

void vmm_init() {
  // We must fetch and store the physical address of the kernel-space
  // page table via the recursive entry. We will need this to copy it
  // into all new address spaces.

  // Kernel-space in x86_64 lives in a single PDPT (a single 512GB
  // block), so we need to save the address of this single PDPT.
  const PageTableEntry *PML4T = get_PML4T();
  // convert the kernel start address to an index
  KSPACE_PDPT_PHYS_ADDR = PTE_get_addr(PML4T[KSPACE_PML4T_INDEX]);
}

static inline uintptr_t round_up_addr(uintptr_t addr) {
  const uintptr_t remainder = addr % PAGE_SIZE;
  if (remainder == 0) return addr;
  return addr + (PAGE_SIZE - remainder);
}

static inline uintptr_t round_down_addr(uintptr_t addr) {
  const uintptr_t remainder = addr % PAGE_SIZE;
  return addr - remainder;
}

static inline size_t pages_needed_for_size(size_t size) {
  const size_t pages = size / PAGE_SIZE;
  const size_t remainder = size % PAGE_SIZE;
  if (remainder == 0) return pages;
  return pages + 1;
}

// is_virt_addr_used is used for finding free virtual address ranges
// for mapping. It's designed to be used independently of how free
// addresses are tracked -- whether that's just the presence of a
// mapped page, or whether the kernel permits lazy mapping via
// VMAs. The implementation must be updated to match.
static bool is_virt_addr_used(const uintptr_t virt_addr) {
  const struct ptindices is = virt_addr_to_ptindices(virt_addr);
  const PageTableEntry *pml4t = get_PML4T();
  if (!pml4t[is.pml4t_i].present) return false;
  const PageTableEntry *pdpt = get_PDPT(is.pml4t_i);
  if (!pdpt[is.pdpt_i].present) return false;
  if (pdpt[is.pdpt_i].page_size) return true;
  const PageTableEntry *pd = get_PD(is.pml4t_i, is.pdpt_i);
  if (!pd[is.pd_i].present) return false;
  if (pd[is.pd_i].page_size) return true;
  const PageTableEntry *pt = get_PT(is.pml4t_i, is.pdpt_i, is.pd_i);
  return pt[is.pt_i].present;
}

void *vmm_kmap(uintptr_t phys_addr, const size_t size, uintptr_t virt_addr,
               flags_t flags) {
  // adjust the target address to get our starting point
  virt_addr = round_up_addr(virt_addr);
  if (virt_addr < KERNEL_VADDR_START) {
    virt_addr = KERNEL_VADDR_START;
  }

  // get the address of the physical page that phys_addr lies within
  const uintptr_t phys_page = round_down_addr(phys_addr);
  const uintptr_t phys_offset = phys_addr - phys_page;

  const size_t pages_required = pages_needed_for_size(size);

  size_t found_free_pages = 0;
  uintptr_t start_addr = virt_addr;

  while (found_free_pages < pages_required && virt_addr < KSPACE_END) {
    if (is_virt_addr_used(virt_addr)) {
      // reset
      virt_addr += PAGE_SIZE;
      found_free_pages = 0;
      start_addr = virt_addr;
      continue;
    }
    ++found_free_pages;
    virt_addr += PAGE_SIZE;
  }

  if (found_free_pages < pages_required) kpanic("vmm_kmap could not find enough free pages\nRequested: %zu\n", pages_required);

  // TODO: create the mapping in the page table

  return (void*)start_addr + phys_offset;
}
