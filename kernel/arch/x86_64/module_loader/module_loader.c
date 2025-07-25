#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "The module loader needs to be compiled with a ix86-elf compiler"
#endif

#include <stdio.h>
#include <multiboot.h>
#include <kernel/tty.h>
#include "elf_paged.h"
#include "addr_checker.h"
#include "bump_alloc.h"

extern const size_t __loader_end;

#define PREV_LINE term_get_pos_y()-1

/* We'll define the page tables needed to identity map the first 8MB
   of memory in long mode. This requires four page tables, one PDT,
   one PDPT, and one PML4T. */

// define some constants for working with page tables
#define sz_PT 512
#define PT_ATTRS __attribute__((aligned(4096))) __attribute__((section(".bss")))

// PML4T
volatile uint64_t page_level_4_tab[sz_PT] PT_ATTRS;
// PML4T[0]
volatile static uint64_t page_dir_ptr_tab[sz_PT] PT_ATTRS;
// PML4T[0][0]: This is our initial identity map
volatile static uint64_t page_dir[sz_PT] PT_ATTRS;

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

  printf("[+] Entered module loader\n");

  if (mis->FLAGS & (1 << 3) && mis->mods_count > 0) {
    printf("[+] Modules are available. Testing module 0...\n");
  } else {
    term_set_fg(4);
    printf("[E] Modules are unavailable. Exiting...\n");
    return;
  }

  // verify that the first module in the list is an ELF file
  const char *mod = (char*)mis->mods[0].mod_start;
  printf("    * %s\n", mis->mods[0].string);
  if (!is_ELF(mod)) {
    term_set_fg(4);
    printf("      Unknown format\n");
    return;
  }
  // print the ELF info
  printf("      ELF %s-bit %s\n",
         (get_ELF_class(mod) == EI_CLASS_32BIT) ? "32" : (get_ELF_class(mod) == EI_CLASS_64BIT ? "64" : "?""?"),
         (get_ELF_endianness(mod) == EI_ENDIANNESS_LITTLE) ? "LE" : (get_ELF_endianness(mod) == EI_ENDIANNESS_BIG ? "BE" : "?""?")
  );
  if (get_ELF_endianness(mod) == EI_ENDIANNESS_BIG) {
    term_set_fg(4);
    term_print_string_at("BE", 17, PREV_LINE);
    return;
  }

  // find the first free address and set up the bump allocator
  const size_t mis_max_addr = (size_t)get_MIS_max_addr(mis);
  const size_t highest_addr = mis_max_addr > __loader_end ? mis_max_addr : __loader_end;
  bump_init(highest_addr + 1);
  printf("[+] Bump allocator set up at 0x%08zx\n", (size_t)bump_malloc(0));

  // load the module
  if (get_ELF_class(mod) == EI_CLASS_32BIT) {
    term_set_fg(4);
    printf("[E] Module loader does not support 32-bit ELFs\n");
    return;
  } else {
    printf("[+] Setting up identity tables... ");
    setup_page_tables();
    printf("Done!\n");
    printf("[+] Mapping ELF... ");
    elf64_map_program_image(mod, page_level_4_tab);
    printf("Done!\n");
    printf("[+] Jumping to long loader...\n");
    return;

    entry.entry64 = get_elf64_entrypoint(mod);
    asm ("call switch_to_long" : : : );
  }
}

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
  page_dir_ptr_tab[0] = (uint64_t)(uintptr_t)(page_dir) | 3;
  page_level_4_tab[0] = (uint64_t)(uintptr_t)(page_dir_ptr_tab) | 3;
}
