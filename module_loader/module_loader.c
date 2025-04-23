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
    const char *mod = (char*)mis->mods[i].mod_start;
    print_string("    * ", 0, yline);
    print_string(mis->mods[i].string, 6, yline);
    ++yline;
    if (!is_ELF(mod)) {
      print_string("      Unknown format", 0, yline);
      ++yline;
      continue;
    }
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
  }

  // load the first module
  const char *mod = (char*)mis->mods[0].mod_start;
  if (get_ELF_class(mod) == EI_CLASS_32BIT) {
    elf32_build_program_image(mod);
    print_string("[+] ELF Loaded!", 0, yline);
    ++yline;
    print_string("[+] Jumping to entrypoint...", 0, yline);
    ++yline;
    void (*entry)() = (void (*)())get_elf32_entrypoint(mod);
    entry();
  } else {
    terminal_color.fg = 4;
    print_string("[E] Unsupported ELF format. Exiting...", 0, yline);
  }
}
