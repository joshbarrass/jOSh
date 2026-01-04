#include <kernel/memory/vmm.h>
#include <kernel/memory/constants.h>
#include <kernel/x86_64/memory/paging.h>
#include <kernel/x86_64/memory/recursive_pt.h>
#include <kernel/panic.h>

#ifdef VERBOSE_VMM
#include <stdio.h>
#endif

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

static inline size_t pages_needed_for_size(const size_t size) {
  const size_t pages = size / PAGE_SIZE;
  const size_t remainder = size % PAGE_SIZE;
  if (remainder == 0) return pages;
  return pages + 1;
}

// vmm_get_phys is implemented via a static inline function so that
// the code can be reused without the overhead of a function call in
// is_virt_addr_used
static inline __attribute__((always_inline)) phys_addr_t _vmm_get_phys(const virt_addr_t virt_addr) {
  const struct ptindices is = virt_addr_to_ptindices(virt_addr);
  const PageTableEntry *pml4t = get_PML4T();
  if (!pml4t[is.pml4t_i].present) return (phys_addr_t)(-1);
  const PageTableEntry *pdpt = get_PDPT(is.pml4t_i);
  if (!pdpt[is.pdpt_i].present) return (phys_addr_t)(-1);
  if (pdpt[is.pdpt_i].page_size) return PTE_get_addr(pdpt[is.pdpt_i]);
  const PageTableEntry *pd = get_PD(is.pml4t_i, is.pdpt_i);
  if (!pd[is.pd_i].present) return (phys_addr_t)(-1);
  if (pd[is.pd_i].page_size) return PTE_get_addr(pd[is.pd_i]);
  const PageTableEntry *pt = get_PT(is.pml4t_i, is.pdpt_i, is.pd_i);
  if (!pt[is.pt_i].present) return (phys_addr_t)(-1);
  return PTE_get_addr(pt[is.pt_i]);
}

phys_addr_t vmm_get_phys(const virt_addr_t virt_addr) {
  return _vmm_get_phys(virt_addr);
}

// is_virt_addr_used is used for finding free virtual address ranges
// for mapping. It's designed to be used independently of how free
// addresses are tracked -- whether that's just the presence of a
// mapped page, or whether the kernel permits lazy mapping via
// VMAs. The implementation must be updated to match.
static bool is_virt_addr_used(const virt_addr_t virt_addr) {
  // currently, we can just reuse the code underlying vmm_get_phys; if
  // vmm_get_phys reports -1, then we know the address isn't mapped
  return _vmm_get_phys(virt_addr) != (phys_addr_t)(-1);
}

virt_addr_t vmm_kmap(phys_addr_t phys_addr, const size_t size, virt_addr_t virt_addr,
               flags_t flags) {
  // adjust the target address to get page-aligned starting point
  virt_addr = (virt_addr_t)round_up_addr((uintptr_t)virt_addr);
  if ((uintptr_t)virt_addr < KERNEL_VADDR_START) {
    virt_addr = (virt_addr_t)KERNEL_VADDR_START;
  }

  // get the range of the physical pages we need to use
  const phys_addr_t phys_page_end = round_up_addr(phys_addr + size);
  const phys_addr_t phys_page = round_down_addr(phys_addr);
  const uintptr_t phys_offset = phys_addr - phys_page;

  const size_t pages_required = pages_needed_for_size(phys_page_end - phys_page);
  #ifdef VERBOSE_VMM
  printf("Need %zu pages\n", pages_required);
  #endif

  size_t found_free_pages = 0;
  virt_addr_t start_addr = virt_addr;

  while (found_free_pages < pages_required && (uintptr_t)virt_addr < KSPACE_END) {
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

  // create the mapping in the page table
  for (size_t i = 0; i < pages_required; ++i) {
    const virt_addr_t addr = start_addr + i*PAGE_SIZE;
    const phys_addr_t page_addr = phys_page + i*PAGE_SIZE;
    const PageTableEntry entry =
        {true,  true, false,      false, false, false, false, false,
         false, 0,    page_addr >> 12, 0,     0,     0,     false}; // TODO: writeable should be controllable.
    if (!create_page_table_entry(addr, entry)) {
      kpanic("vmm_kmap: failed to create page table entry");
    }
  }

  return (void*)start_addr + phys_offset;
}

void vmm_kunmap(virt_addr_t virt_addr, const size_t size) {
  // ensure the addresses are page-aligned and cover everything they
  // should
  const uintptr_t end_addr = round_up_addr((uintptr_t)virt_addr + size);
  virt_addr = (virt_addr_t)round_down_addr((uintptr_t)virt_addr);

  // TODO: this is fine and plenty simple for now and for kspace, but
  // in the future it may be worth freeing up the page tables if
  // they're no longer needed.
  for (; (uintptr_t)virt_addr < end_addr; virt_addr += PAGE_SIZE) {
    const struct ptindices is = virt_addr_to_ptindices(virt_addr);
    get_PT(is.pml4t_i, is.pdpt_i, is.pd_i)[is.pt_i].present = false;
#ifdef VERBOSE_VMM
    printf("Unmapped %#18zx\n", virt_addr);
#endif
    invlpg(virt_addr);
  }
}
