#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "The module loader needs to be compiled with a ix86-elf compiler"
#endif

#include <multiboot.h>
#include <tty.h>
#include "elf.h"
#include "addr_checker.h"

const MIS *mis;
typedef union {
  uint64_t entry64;
  uint32_t entry32;
} Entrypoint;
Entrypoint entry;
unsigned int yline = 0;

static void setup_page_tables();

void module_loader_main() {
  clear_screen();
  terminal_color.fg = 10;

  print_string("[+] Entered module loader", 0, yline);
  ++yline;

  if (mis->FLAGS & (1 << 3) && mis->mods_count > 0) {
    print_string("[+] Modules are available. Testing module 0...", 0, yline);
    ++yline;
  } else {
    terminal_color.fg = 4;
    print_string("[E] Modules are unavailable. Exiting...", 0, yline);
    return;
  }

  // verify that the first module in the list is an ELF file
  const char *mod = (char*)mis->mods[0].mod_start;
  print_string("    * ", 0, yline);
  print_string(mis->mods[0].string, 6, yline);
  ++yline;
  if (!is_ELF(mod)) {
    terminal_color.fg = 4;
    print_string("      Unknown format", 0, yline);
    ++yline;
    return;
  }
  // print the ELF info
  print_string("      ELF ??-bit ??", 0, yline);
  if (get_ELF_class(mod) == EI_CLASS_32BIT) {
    print_string("32", 10, yline);
  } else if (get_ELF_class(mod) == EI_CLASS_64BIT) {
    print_string("64", 10, yline);
  }
  if (get_ELF_endianness(mod) == EI_ENDIANNESS_LITTLE) {
    print_string("LE", 17, yline);
  } else if (get_ELF_endianness(mod) == EI_ENDIANNESS_BIG) {
    terminal_color.fg = 4;
    print_string("BE", 17, yline);
    return;
  }
  ++yline;

  // determine the lowest virtual address of the ELF
  uint64_t lowest_addr;
  if (get_ELF_class(mod) == EI_CLASS_32BIT) {
    lowest_addr = (uint64_t)get_elf32_lowest_addr(mod);
  } else {
    lowest_addr = get_elf64_lowest_addr(mod);
  }

  // check that the ELF can be safely loaded to that address
  if (!check_all(lowest_addr, mis)) {
    terminal_color.fg = 4;
    print_string("[E] ELF cannot be moved safely!", 0, yline);
    return;
  }

  // load the module
  if (get_ELF_class(mod) == EI_CLASS_32BIT) {
    elf32_build_program_image(mod);
    print_string("[+] ELF Loaded!", 0, yline);
    ++yline;
    print_string("[+] Jumping to entrypoint...", 0, yline);
    ++yline;

    // make the call using inline assembly
    // this way, we can guarantee the calling convention that we
    // expect. We can also restore the multiboot loader's inputs at
    // the same time.
    entry.entry32 = get_elf32_entrypoint(mod);
    asm ("jmp *%0\r\n" : : "m"(entry.entry32), "a"(0x2BADB002), "b"(mis) :);
  } else {
    elf64_build_program_image(mod);
    print_string("[+] ELF Loaded!", 0, yline);
    ++yline;
    print_string("[+] Setting up identity pages...", 0, yline);
    ++yline;
    setup_page_tables();
    print_string("[+] Jumping to entrypoint...", 0, yline);
    ++yline;

    entry.entry64 = get_elf64_entrypoint(mod);
    asm ("call switch_to_long" : : : );
  }
}

/* We'll define the page tables needed to identity map the first 8MB
   of memory in long mode. This requires four page tables, one PDT,
   one PDPT, and one PML4T. */

// define some constants for working with page tables
#define sz_PT 512
uint64_t page_level_4_tab[sz_PT] __attribute__((aligned(4096))) __attribute__((section(".bss")));
static uint64_t page_dir_ptr_tab[sz_PT] __attribute__((aligned(4096))) __attribute__((section(".bss")));
static uint64_t page_dir[sz_PT] __attribute__((aligned(4096))) __attribute__((section(".bss")));

static void zero_page_table(uint64_t *table) {
  for (size_t i = 0; i < sz_PT; ++i) {
    table[i] = 0;
  }
}

static void id_table(uint64_t *table, uint64_t pti) {
  uint64_t start_addr = 0x1000 * sz_PT * pti;
  for (uint64_t i = 0; i < sz_PT; ++i) {
    table[i] = start_addr + (uint64_t)((0x1000 * i) | 3); // attributes: supervisor level, read/write, present
  }
}

static void setup_page_tables() {
  zero_page_table(page_dir);
  zero_page_table(page_dir_ptr_tab);
  zero_page_table(page_level_4_tab);

  // Identity map the first 8MB
  // We can use 2MB pages here for ease/efficiency
  page_dir[0] = (uint64_t)(0) | 3 | 128;
  page_dir[1] = (uint64_t)(0x200000) | 3 | 128;
  page_dir[2] = (uint64_t)(0x400000) | 3 | 128;
  page_dir[3] = (uint64_t)(0x600000) | 3 | 128;
  page_dir_ptr_tab[0] = (uint64_t)(page_dir) | 3;
  page_level_4_tab[0] = (uint64_t)(page_dir_ptr_tab) | 3;
}
