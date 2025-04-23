#include "elf.h"

bool is_ELF(char *addr) {
  return (addr[EI_MAG] == 0x7F && addr[EI_MAG+1] == 'E' && addr[EI_MAG+2] == 'L' && addr[EI_MAG+3] == 'F');
}

char get_ELF_class(char *addr) {
  return addr[EI_CLASS];
}

char get_ELF_endianness(char *addr) {
  return addr[EI_ENDIANNESS];
}
