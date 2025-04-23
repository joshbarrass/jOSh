#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#include "multiboot.h"
#include "tty.h"

const MIS *mis;

void module_loader_main() {
  clear_screen();
  terminal_color.fg = 10;

  unsigned int yline = 0;
  
  print_string("[+] Entered module loader", 0, yline);
  ++yline;

  if (mis->FLAGS & (1 << 3) && mis->mods_count > 0) {
    print_string("[+] Modules are available", 0, yline);
    ++yline;
  } else {
    terminal_color.fg = 4;
    print_string("[E] Modules are unavailable. Exiting...", 0, yline);
    return;
  }

  for (size_t i = 0; i < mis->mods_count; ++i) {
    print_string("    * ", 0, yline);
    print_string(mis->mods[i].string, 6, yline);
  }
}
