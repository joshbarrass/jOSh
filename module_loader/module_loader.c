#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "The module loader needs to be compiled with a ix86-elf compiler"
#endif

#include "multiboot.h"
#include "terminal/tty.h"
#include "elf.h"
#include "addr_checker.h"

#define PREV_LINE term_get_pos_y()-1

const MIS *mis;
typedef union {
  uint64_t entry64;
  uint32_t entry32;
} Entrypoint;
Entrypoint entry;

static void setup_page_tables();

void module_loader_main() {
  term_clear_screen();
  term_set_fg(10);

  term_println("[+] Entered module loader");

  if (mis->FLAGS & (1 << 3) && mis->mods_count > 0) {
    term_println("[+] Modules are available. Testing module 0...");
  } else {
    term_set_fg(4);
    term_println("[E] Modules are unavailable. Exiting...");
    return;
  }

  // verify that the first module in the list is an ELF file
  const char *mod = (char*)mis->mods[0].mod_start;
  term_println("    * ");
  term_print_string_at(mis->mods[0].string, 6, PREV_LINE);
  if (!is_ELF(mod)) {
    term_set_fg(4);
    term_println("      Unknown format");
    return;
  }
  // print the ELF info
  term_println("      ELF ??-bit ??");
  if (get_ELF_class(mod) == EI_CLASS_32BIT) {
    term_print_string_at("32", 10, PREV_LINE);
  } else if (get_ELF_class(mod) == EI_CLASS_64BIT) {
    term_print_string_at("64", 10, PREV_LINE);
  }
  if (get_ELF_endianness(mod) == EI_ENDIANNESS_LITTLE) {
    term_print_string_at("LE", 17, PREV_LINE);
  } else if (get_ELF_endianness(mod) == EI_ENDIANNESS_BIG) {
    term_set_fg(4);
    term_print_string_at("BE", 17, PREV_LINE);
    return;
  }

  // determine the lowest virtual address of the ELF
  uint64_t lowest_addr;
  if (get_ELF_class(mod) == EI_CLASS_32BIT) {
    lowest_addr = (uint64_t)get_elf32_lowest_addr(mod);
  } else {
    lowest_addr = get_elf64_lowest_addr(mod);
  }

  // check that the ELF can be safely loaded to that address
  if (!check_all(lowest_addr, mis)) {
    term_set_fg(4);
    term_println("[E] ELF cannot be moved safely!");
    return;
  }

  // load the module
  if (get_ELF_class(mod) == EI_CLASS_32BIT) {
    elf32_build_program_image(mod);
    term_println("[+] ELF Loaded!");
    term_println("[+] Jumping to entrypoint...");

    // make the call using inline assembly
    // this way, we can guarantee the calling convention that we
    // expect. We can also restore the multiboot loader's inputs at
    // the same time.
    entry.entry32 = get_elf32_entrypoint(mod);
    asm ("jmp *%0\r\n" : : "m"(entry.entry32), "a"(0x2BADB002), "b"(mis) :);
  } else {
    elf64_build_program_image(mod);
    term_println("[+] ELF Loaded!");
    term_println("[+] Setting up identity pages...");
    setup_page_tables();
    term_println("[+] Jumping to entrypoint...");

    entry.entry64 = get_elf64_entrypoint(mod);
    asm ("call switch_to_long" : : : );
  }
}

/* We'll define the page tables needed to identity map the first 8MB
   of memory in long mode. This requires four page tables, one PDT,
   one PDPT, and one PML4T. */

// define some constants for working with page tables
#define sz_PT 512
volatile uint64_t page_level_4_tab[sz_PT] __attribute__((aligned(4096))) __attribute__((section(".bss")));
volatile static uint64_t page_dir_ptr_tab[sz_PT] __attribute__((aligned(4096))) __attribute__((section(".bss")));
volatile static uint64_t page_dir[sz_PT] __attribute__((aligned(4096))) __attribute__((section(".bss")));

static void zero_page_table(volatile uint64_t *table) {
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
