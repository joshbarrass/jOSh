#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "The module loader needs to be compiled with a ix86-elf compiler"
#endif

#include "multiboot.h"
#include "tty.h"
#include "elf.h"

const MIS *mis;
typedef union {
  uint64_t entry64;
  uint32_t entry32;
} Entrypoint;
Entrypoint entry;
unsigned int yline = 0;

void setup_page_tables();

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
    print_string("BE", 17, yline);
  }
  ++yline;

  // load the module
  if (get_ELF_class(mod) == EI_CLASS_32BIT) {
    elf32_build_program_image(mod);
    print_string("[+] ELF Loaded!", 0, yline);
    ++yline;
    print_string("[+] Jumping to entrypoint...", 0, yline);
    ++yline;

    // make the call using inline assembly
    // this way, we can guarantee the calling convention that we
    // expect.
    entry.entry32 = get_elf32_entrypoint(mod);
    asm ("call *%0" : : "r"(entry.entry32) : );
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

uint64_t page_level_4_tab[512] __attribute__((aligned(4096))) __attribute__((section(".bss")));
uint64_t page_dir_ptr_tab[512] __attribute__((aligned(4096))) __attribute__((section(".bss")));
uint64_t page_dir[512] __attribute__((aligned(4096))) __attribute__((section(".bss")));
uint64_t page_table_zero[512] __attribute__((aligned(4096))) __attribute__((section(".bss")));
uint64_t page_table_one[512] __attribute__((aligned(4096))) __attribute__((section(".bss")));
uint64_t page_table_two[512] __attribute__((aligned(4096))) __attribute__((section(".bss")));
uint64_t page_table_three[512] __attribute__((aligned(4096))) __attribute__((section(".bss")));

void zero_page_table(uint64_t *table) {
  for (size_t i = 0; i < 512; ++i) {
    table[i] = 0;
  }
}

void id_table(uint64_t *table, uint64_t pti) {
  uint64_t start_addr = 0x1000 * 512 * pti;
  for (uint64_t i = 0; i < 512; ++i) {
    table[i] = start_addr + (uint64_t)((0x1000 * i) | 3); // attributes: supervisor level, read/write, present
  }
}

void setup_page_tables() {
  zero_page_table(page_dir);
  zero_page_table(page_dir_ptr_tab);
  zero_page_table(page_table_zero);
  zero_page_table(page_table_one);
  zero_page_table(page_table_two);
  zero_page_table(page_table_three);

  id_table(page_table_zero, 0);
  id_table(page_table_one, 1);
  id_table(page_table_two, 2);
  id_table(page_table_three, 3);

  page_dir[0] = (uint64_t)(page_table_zero) | 3;
  page_dir[1] = (uint64_t)(page_table_one) | 3;
  page_dir[2] = (uint64_t)(page_table_two) | 3;
  page_dir[3] = (uint64_t)(page_table_three) | 3;
  page_dir_ptr_tab[0] = (uint64_t)(page_dir) | 3;
  page_level_4_tab[0] = (uint64_t)(page_dir_ptr_tab) | 3;
}
