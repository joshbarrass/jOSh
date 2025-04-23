#ifndef ELF_H
#define ELF_H 1

/* Source: http://www.skyfree.org/linux/references/ELF_Format.pdf

   Adapted from above based on
   https://en.wikipedia.org/wiki/Executable_and_Linkable_Format#ELF_header */

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef  int32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

#define EI_NIDENT 16
#define EI_MAG 0x0
#define EI_CLASS 0x4
#define EI_ENDIANNESS 0x5
#define EI_VERSION 0x6
#define EI_OSABI 0x7
#define EI_ABIVERSION 0x8
#define EI_PAD 0x9

#define EI_CLASS_32BIT 1
#define EI_CLASS_64BIT 2

#define EI_ENDIANNESS_LITTLE 1
#define EI_ENDIANNESS_BIG 2

typedef struct {
unsigned char e_ident[EI_NIDENT];
Elf32_Half e_type;
Elf32_Half e_machine;
Elf32_Word e_version;
Elf32_Addr e_entry;
Elf32_Off e_phoff;
Elf32_Off e_shoff;
Elf32_Word e_flags;
Elf32_Half e_ehsize;
Elf32_Half e_phentsize;
Elf32_Half e_phnum;
Elf32_Half e_shentsize;
Elf32_Half e_shnum;
Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef uint64_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef uint64_t Elf64_Off;
typedef  int32_t Elf64_Sword;
typedef uint32_t Elf64_Word;

typedef struct {
unsigned char e_ident[EI_NIDENT];
Elf64_Half e_type;
Elf64_Half e_machine;
Elf64_Word e_version;
Elf64_Addr e_entry;
Elf64_Off e_phoff;
Elf64_Off e_shoff;
Elf64_Word e_flags;
Elf64_Half e_ehsize;
Elf64_Half e_phentsize;
Elf64_Half e_phnum;
Elf64_Half e_shentsize;
Elf64_Half e_shnum;
Elf64_Half e_shstrndx;
} Elf64_Ehdr;

bool is_ELF(char *addr);
char get_ELF_class(char *addr);
char get_ELF_endianness(char *addr);

#endif
