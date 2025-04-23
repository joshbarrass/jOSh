#include "elf.h"

// provide our own memmove here for ease
// this may get replaced later
void memmove(void *dest, const void *src, size_t n) {
  const char *s = (char*)src;
  char *d = (char*)dest;

  if (s > d) {
    // data starts in front of the destination
    // copy from start to end to avoid overwriting
    for (size_t i = 0; i < n; ++i) {
      d[i] = s[i];
    }
  } else {
    // data starts behind the destination
    // copy from end to start
    for (size_t i = n; i > 0; --i) {
      d[i-1] = s[i-1];
    }
  }
}

bool is_ELF(const char * const addr) {
  return (addr[EI_MAG] == 0x7F && addr[EI_MAG+1] == 'E' && addr[EI_MAG+2] == 'L' && addr[EI_MAG+3] == 'F');
}

char get_ELF_class(const char * const addr) {
  return addr[EI_CLASS];
}

char get_ELF_endianness(const char * const addr) {
  return addr[EI_ENDIANNESS];
}

void elf32_build_program_image(const char *const elf) {
  const Elf32_Ehdr * const header = (const Elf32_Ehdr * const)elf;
  const uint64_t p = (uint64_t)elf + (uint64_t)header->e_phoff;
  const Elf32_Phdr * const pheader = (const Elf32_Phdr * const)p;

  for (size_t i = 0; i < header->e_phnum; ++i) {
    const void *const segment_start = (const void *const)(pheader[i].p_offset + elf);
    void *const virt_addr = (const void *const)pheader[i].p_vaddr;
    const size_t fsize = (size_t)pheader[i].p_filesz;
    const size_t msize = (size_t)pheader[i].p_memsz;

    memmove(virt_addr, segment_start, fsize);
    for (int i = fsize; i < msize; ++i) {
      ((char*)virt_addr)[i] = 0;
    }
  }
}

Elf32_Addr get_elf32_entrypoint(const char *const elf) {
  const Elf32_Ehdr * const header = (const Elf32_Ehdr * const)elf;
  return header->e_entry;
}
