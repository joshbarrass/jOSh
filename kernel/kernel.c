#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_64 1
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#define ARCH_32 1
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/vga.h>
#include <multiboot.h>
#include <kernel/panic.h>
#include <archdef.h>
#include <kernel/mmap.h>

const MIS *mis = NULL;

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
  term_info_color();
  term_clear_screen();
  print_welcome_message();
  if (mis == NULL) {
    term_error_color();
    printf("[!] Missing multiboot information struct!\n");
    return;
  }

  printf("[*] Checking required multiboot flags...\n");
  if (mis->FLAGS & MULTIBOOT_FLAG_FULL_MMAP) {
    term_good_color();
    printf("[+] Memory map is available!\n");
    term_info_color();
  } else {
    term_error_color();
    printf("[!] Memory map unavailable!\n");
    return;
  }

  // ensure the memory map has not been corrupted
  // if below 2MB, none of our loaded data could have collided with it
  if (mis->mmap >= 2 * 1024 * 1024 || mis->mmap + mis->mmap_length >= 2 * 1024 * 1024) { 
    term_error_color();
    printf("[!] Memory map corrupted!\n");
    return;
  } else {
    term_good_color();
    printf("[+] Intact mmap found at %#018zx\n", mis->mmap);
    term_info_color();
  }

  // print the memory map
  printf("    |       start       -         end       |type| raw|\n");
  size_t adv = 0;
  mmap *map = get_mmap(mis);
  for (size_t i = 0; i < mis->mmap_length; i += adv) {
    mmap *entry = (mmap*)(&((char*)map)[i]);
    adv = entry->size+sizeof(entry->size);
    printf("    |0x%016zX - 0x%016zX|%s|0x%02x|\n", entry->base_addr, entry->base_addr+entry->length-1, get_mmap_type_string(entry->type), entry->type);
  }

  return;
}
