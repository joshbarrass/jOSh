#include <elf.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/constants.h>
#include <string.h>

bool is_ELF(const char * const addr) {
  return (addr[EI_MAG] == 0x7F && addr[EI_MAG+1] == 'E' && addr[EI_MAG+2] == 'L' && addr[EI_MAG+3] == 'F');
}

char get_ELF_class(const char * const addr) {
  return addr[EI_CLASS];
}

char get_ELF_endianness(const char * const addr) {
  return addr[EI_ENDIANNESS];
}

/* void elf32_build_program_image(const char *const elf) { */
/*   const Elf32_Ehdr * const header = (const Elf32_Ehdr * const)elf; */
/*   const uint64_t p = (uint64_t)(uintptr_t)elf + (uint64_t)(uintptr_t)header->e_phoff; */
/*   const Elf32_Phdr * const pheader = (const Elf32_Phdr * const)(uintptr_t)p; */

/*   for (size_t i = 0; i < header->e_phnum; ++i) { */
/*     const void *const segment_start = (const void *const)(pheader[i].p_offset + elf); */
/*     void *const virt_addr = (void *const)(uintptr_t)pheader[i].p_vaddr; */
/*     const size_t fsize = (size_t)pheader[i].p_filesz; */
/*     const size_t msize = (size_t)pheader[i].p_memsz; */

/*     vmm_kmap(segment_start,  */
/*     memmove(virt_addr, segment_start, fsize); */
/*     for (int i = fsize; i < msize; ++i) { */
/*       ((char*)virt_addr)[i] = 0; */
/*     } */
/*   } */
/* } */

void elf64_build_program_image(const char *const elf) {
  const Elf64_Ehdr * const header = (const Elf64_Ehdr * const)elf;
  const uint64_t p = (uint64_t)(uintptr_t)elf + (uint64_t)(uintptr_t)header->e_phoff;
  const Elf64_Phdr * const pheader = (const Elf64_Phdr * const)(uintptr_t)p;

  for (size_t i = 0; i < header->e_phnum; ++i) {
    const void *const segment_start = (const void *const)(pheader[i].p_offset + elf);
    void *const virt_addr = (void *const)(uintptr_t)pheader[i].p_vaddr;
    const size_t fsize = (size_t)pheader[i].p_filesz;
    const size_t msize = (size_t)pheader[i].p_memsz;

    size_t pages_mapped = 0;

    // find how much of the segment is not page-aligned
    const size_t size_to_map = (fsize / PAGE_SIZE) * PAGE_SIZE;
    const size_t physical_remainder = fsize % PAGE_SIZE;
    // map the page-aligned chunk directly if there is one to map
    if (size_to_map > 0) vmm_kmap((uintptr_t)segment_start, size_to_map, (uintptr_t)virt_addr, 0);
    pages_mapped += size_to_map / PAGE_SIZE;
    if (physical_remainder != 0) {
      // copy the remainder to a new page and map that contiguously
      const uintptr_t newpage = (uintptr_t)pmm_alloc_pages(1);
      vmm_kmap(newpage, PAGE_SIZE, (uintptr_t)virt_addr + size_to_map, 0);
      memmove((void*)((uintptr_t)virt_addr + size_to_map), segment_start + size_to_map, physical_remainder);
      for (int i = physical_remainder; i < PAGE_SIZE; ++i) {
        ((char*)((uintptr_t)virt_addr + size_to_map))[i] = 0;
      }
      ++pages_mapped;
    }

    // generate enough pages for the remaining msize
    size_t total_pages_needed = msize / PAGE_SIZE;
    const size_t rem = msize % PAGE_SIZE;
    if (rem > 0) total_pages_needed += 1;
    const size_t pages_to_alloc = total_pages_needed - pages_mapped;
    const uintptr_t newpage = (uintptr_t)pmm_alloc_pages(pages_to_alloc);
    vmm_kmap(newpage, pages_to_alloc*PAGE_SIZE, (uintptr_t)virt_addr + fsize, 0);
  }
}

Elf32_Addr get_elf32_entrypoint(const char *const elf) {
  const Elf32_Ehdr * const header = (const Elf32_Ehdr * const)elf;
  return header->e_entry;
}
Elf64_Addr get_elf64_entrypoint(const char *const elf) {
  const Elf64_Ehdr * const header = (const Elf64_Ehdr * const)elf;
  return header->e_entry;
}

Elf32_Addr get_elf32_lowest_addr(const char *const elf) {
  const Elf32_Ehdr * const header = (const Elf32_Ehdr * const)elf;
  const uint64_t p = (uint64_t)(uintptr_t)elf + (uint64_t)(uintptr_t)header->e_phoff;
  const Elf32_Phdr * const pheader = (const Elf32_Phdr * const)(uintptr_t)p;
  Elf32_Addr lowest_addr = 0xffffffff;

  for (size_t i = 0; i < header->e_phnum; ++i) {
    Elf32_Addr virt_addr = pheader[i].p_vaddr;
    if (virt_addr < lowest_addr) {
      lowest_addr = virt_addr;
    }
  }
  return lowest_addr;
}

Elf64_Addr get_elf64_lowest_addr(const char *const elf) {
  const Elf64_Ehdr * const header = (const Elf64_Ehdr * const)elf;
  const uint64_t p = (uint64_t)(uintptr_t)elf + (uint64_t)(uintptr_t)header->e_phoff;
  const Elf64_Phdr * const pheader = (const Elf64_Phdr * const)(uintptr_t)p;
  Elf64_Addr lowest_addr = 0xffffffffffffffff;

  for (size_t i = 0; i < header->e_phnum; ++i) {
    Elf64_Addr virt_addr = pheader[i].p_vaddr;
    if (virt_addr < lowest_addr) {
      lowest_addr = virt_addr;
    }
  }
  return lowest_addr;
}
