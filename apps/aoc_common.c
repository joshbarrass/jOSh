#include <string.h>
#include <multiboot.h>
#include <kernel/memory/constants.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>

extern const MIS *mis;

static const char *empty_string = "\0";

char *get_input() {
  const Mod *mods = get_mods(mis);
  for (size_t i = 0; i < mis->mods_count; ++i) {
    const Mod *mod = mods + i;
    if (strcmp("input", get_mod_string(mod)) == 0) {
      return (char*)get_mod_start(mod);
    }
  }
  return empty_string;
}
