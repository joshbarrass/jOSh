#include "elf_paged.h"
#include "bump_alloc.h"
#include "paging.h"
#include <stdio.h>

#define ERR_OFF_NOT_PAGE_ALIGNED -1
#define ERR_VIR_NOT_PAGE_ALIGNED -2
#define ERR_FSIZE_NOT_PAGE -3
#define ERR_MSIZE_NOT_PAGE -4
#define ERR_OTHER -255

uint64_t *bump_malloc_pt() {
  bump_align(PAGE_ALIGNMENT);
  uint64_t *addr = bump_malloc(PAGE_SIZE);
  return addr;
}

int elf64_map_program_image(const char *const elf, uint64_t *const pml4t) {
  // Assumes all page tables were zeroed at allocation, and all
  // allocated page tables have been added to the page table tree.
  // This way, a non-zero entry in any page table -> a page table
  // which has already been allocated. If we need to use an
  // unallocated page table, we use the bump allocator to create it.
  //
  // Returns:
  //    0 if program mapped successfully
  //   -1 if a program segment was encountered with an offset address
  //      which is not page-aligned
  //   -2 if a program segment was encountered with a virtual address
  //      which is not page-aligned
  //   -3 if a program segment was encountered with a fsize which is
  //      not a multiple of the page size
  //   -4 if a program segment was encountered with a msize which is
  //      not a multiple of the page size
  const Elf64_Ehdr * const header = (const Elf64_Ehdr * const)elf;
  const uint64_t p = (uint64_t)(uintptr_t)elf + (uint64_t)(uintptr_t)header->e_phoff;
  const Elf64_Phdr * const pheader = (const Elf64_Phdr * const)(uintptr_t)p;

  for (size_t i = 0; i < header->e_phnum; ++i) {
    const void *const segment_start = (const void *const)(pheader[i].p_offset + elf);
    if ((size_t)segment_start % PAGE_ALIGNMENT != 0) return ERR_OFF_NOT_PAGE_ALIGNED;
    const uint64_t virt_addr = pheader[i].p_vaddr;
    if ((uint64_t)(uintptr_t)virt_addr % PAGE_ALIGNMENT != 0) return ERR_VIR_NOT_PAGE_ALIGNED;
    const size_t fsize = (size_t)pheader[i].p_filesz;
    if (fsize % PAGE_ALIGNMENT != 0) return ERR_FSIZE_NOT_PAGE;
    const size_t msize = (size_t)pheader[i].p_memsz;
    if (msize % PAGE_ALIGNMENT != 0) return ERR_MSIZE_NOT_PAGE;

    // map the contents of the file into memory one page at a time
    for (uint64_t j = 0; j < msize; j += PAGE_SIZE) {
      // get the page table indices corresponding to the virtual address
      size_t i0, i1, i2, i3;
      if (virtual_to_page_table_indices(virt_addr+j, &i0, &i1, &i2, &i3) != 0) return ERR_OTHER;

      // ensure all the tables exist
      uint64_t *pdpt = fetch_page_table(pml4t, i0);
      if (pdpt == NULL) {
        pml4t[i0] = (uint64_t)bump_malloc_pt() | 3;
        pdpt = fetch_page_table(pml4t, i0);
        zero_page_table(pdpt);
      }
      uint64_t *pd = fetch_page_table(pdpt, i1);
      if (pd == NULL) {
        pdpt[i1] = (uint64_t)bump_malloc_pt() | 3;
        pd = fetch_page_table(pdpt, i1);
        zero_page_table(pd);
      }
      uint64_t *pt = fetch_page_table(pd, i2);
      if (pt == NULL) {
        pd[i2] = (uint64_t)bump_malloc_pt() | 3;
        pt = fetch_page_table(pd, i2);
        zero_page_table(pt);
      }

      if (j < fsize) { // still inside the real file
        // point the table to the physical address of the real ELF
        // content
        pt[i3] = (uint64_t)(segment_start + j) | 3;
      } else { // nobits data
        // allocate an empty page to store the data and point the
        // table to it
        pt[i3] = (uint64_t)(bump_malloc(PAGE_SIZE)) | 3;
      }
    }
  }

  return 0;
}
