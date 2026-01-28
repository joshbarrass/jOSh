#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_64 1
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#define ARCH_32 1
#endif

#define VERTLINE "\xb3"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/drivers/ega/ega.h>
#include <kernel/vga.h>
#include <kernel/bootstruct.h>
#include <multiboot2.h>
#include <kernel/panic.h>
#include <archdef.h>
#include <kernel/mmap.h>
#include <kernel/interrupts.h>
#include <kernel/memory/types.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>

#define BS_IS_PRESENT (bootstruct != NULL)

const BootStruct *bootstruct = NULL;
const M2IS *mis = NULL;
static const m2is_meminfo *mis_meminfo = NULL;
static const m2is_mmap *mis_mmap = NULL;
static const m2is_framebuffer_info *mis_framebuffer = NULL;
#define M2IS_CHECK(var)                                                        \
  if (var == NULL) {                                                           \
    term_error_color();                                                        \
    printf("[!] Could not find " #var "\n");                                    \
    return;                                                                    \
  }

static void print_welcome_message() {
  printf("Welcome to jOSh! (%s)\n",
#ifdef ARCH_64
         "64-bit"
#elif ARCH_32
         "32-bit"
#else
         "unknown arch"
#endif
         );
}

inline static void term_info_color() {
  term_set_fg(VGA_COLOR_LIGHT_GREY);
}

inline static void term_good_color() {
  term_set_fg(VGA_COLOR_LIGHT_GREEN);
}

inline static void term_error_color() {
  term_set_fg(VGA_COLOR_RED);
}

void kernel_main() {
  VGA_set_blink(false);
  set_default_console_driver(ega_driver_init((void*)VGA_FRAMEBUFFER_ADDR, 80, 25, 160));
  init_default_term();
  term_info_color();
  term_clear();
  print_welcome_message();
  if (BS_IS_PRESENT) {
    term_good_color();
    printf("[+] Boot struct is available!\n");
    term_info_color();
  } else {
    term_error_color();
    printf("[!] Missing boot struct!\n");
    term_info_color();
  }

  if (mis == NULL) {
    term_error_color();
    printf("[!] Missing multiboot information struct!\n");
    return;
  }

  term_good_color();
  setup_interrupts();
  printf("[+] Loaded interrupts\n");
  term_info_color();

  printf("[*] Searching for required multiboot2 tags...\n");
  {
    m2is_tag_iterator iter = new_m2is_iterator(mis);
    const m2is_tag *tag = m2is_iterator_next(&iter);
    while (tag != NULL) {
      switch (tag->type) {
      case M2IS_TYPE_MEMINFO:
        mis_meminfo = (const m2is_meminfo *)tag;
        break;
      case M2IS_TYPE_MEMMAP:
        mis_mmap = (const m2is_mmap *)tag;
        break;
      case M2IS_TYPE_FRAMEBUFFER:
        mis_framebuffer = (const m2is_framebuffer_info *)tag;
        break;
      default:
      }

      tag = m2is_iterator_next(&iter);
    }
  }
  // check that we found everything
  M2IS_CHECK(mis_meminfo);
  M2IS_CHECK(mis_mmap);
  M2IS_CHECK(mis_framebuffer);

  // print total available memory
  const size_t total_memory = mis_meminfo->mem_lower + mis_meminfo->mem_upper; // KiB
  printf("[*] Total available memory: %zuKiB\n", total_memory);
  printf("    * Low memory:  %zuKiB\n", (size_t)mis_meminfo->mem_lower);
  printf("    * High memory: %zuKiB\n", (size_t)mis_meminfo->mem_upper);

  // print the memory map
  printf("    "VERTLINE"       start       -         end       "VERTLINE"type"VERTLINE" raw"VERTLINE"\n");
  {
    mmap_iterator iter = new_mmap_iterator(mis_mmap);
    for (mmap_entry *entry = mmap_iterator_next(&iter); entry != NULL;
         entry = mmap_iterator_next(&iter)) {
      printf("    " VERTLINE "0x%016zX - 0x%016zX" VERTLINE "%s" VERTLINE
             "0x%02x" VERTLINE "\n",
             entry->base_addr, entry->base_addr + entry->length - 1,
             get_mmap_type_string(entry->type), entry->type);
    }
  }

  // retrieve the first free address from the boot struct, if present
  phys_addr_t lowest_free_page;
  if (BS_IS_PRESENT && bootstruct->flags & BS_FLAG_FREEADDR) {
    const phys_addr_t lowest_free_addr = bs_get_lowest_free_addr(bootstruct);
    lowest_free_page = (uintptr_t)lowest_free_addr;
    const uintptr_t page_diff = lowest_free_page % 0x1000;
    if (page_diff != 0) {
      lowest_free_page += 0x1000-page_diff;
    }
    printf("[*] First free address at: 0x%016zX (%zuKiB)\n[*] First free page at:    0x%016zX (%zuKiB)\n",
           (uintptr_t)lowest_free_addr, (uintptr_t)lowest_free_addr/1024, lowest_free_page, lowest_free_page/1024);
    printf("[*] Total free memory: %zuKiB\n", (uintptr_t)total_memory-lowest_free_page/1024);
  } else {
    // currently have no alternative implementation to work around
    // this -- just panic
    kpanic("Lowest free page info unavailable.\n\n"
           "Bootstruct is present? %d\n"
           "Bootstruct flags: %d\n",
           BS_IS_PRESENT, bootstruct->flags);
    return;
  }

  pmm_init(lowest_free_page, mis_mmap);
  vmm_init();

  // print framebuffer info
  printf("Framebuffer type: %d\n", mis_framebuffer->type);
  printf("Framebuffer dims: %dx%d\n", mis_framebuffer->width, mis_framebuffer->height);
  printf("Framebuffer pitch: %d\n", mis_framebuffer->pitch);

  return;
}
