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

void kernel_main() {
  VGA_set_blink(false);
  term_clear_screen();
  print_welcome_message();
  if (mis != NULL) {
    printf("%s\n\n", get_mod_string(&get_mods(mis)[0]));
  }
  return;
}
