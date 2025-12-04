#include <string.h>
#include <multiboot.h>
#include <kernel/memory/constants.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>

extern const MIS *mis;

static const char *empty_string = "\0";

char *get_input() {
  const Mod *mods = get_mods(mis);
  const Mod *input_mod = NULL;
  for (size_t i = 0; i < mis->mods_count; ++i) {
    const Mod *mod = mods + i;
    if (strcmp("input", get_mod_string(mod)) == 0) {
      input_mod = mod;
    }
  }
  if (input_mod == NULL) return empty_string;
  
  // allocate sufficient space for the input file + a null terminator
  const size_t required_len = (input_mod->mod_end - input_mod->mod_start) + 1;
  size_t pages_required = required_len / PAGE_SIZE;
  if (required_len % PAGE_SIZE != 0) ++pages_required;
  char *new_page = vmm_kmap((uintptr_t)pmm_alloc_pages(pages_required), required_len, 0, 0);
  memmove(new_page, (void*)(uintptr_t)input_mod->mod_start, required_len - 1);
  new_page[required_len-1] = 0;
  return new_page;
}
