#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"
#include "addr_checker.h"

size_t strlen(const char *c) {
  size_t len = 0;
  while (c[len] != 0) {
    ++len;
  }
  return len;
}

// TODO: modify these functions to return the highest address
// found. This will be useful for the future when we want to use
// paging to relocate the kernel instead of physically relocating
// it. This information can be used to help us find free page frames
// for allocating any "no data" sections (e.g. .bss)

bool check_MIS(const uint64_t load_addr, const MIS *mis) {
  return load_addr > ((uint64_t)mis + (uint64_t)sizeof(MIS));
}

bool check_cmdline(const uint64_t load_addr, const MIS *mis) {
  const char *cmdline = get_cmdline(mis);
  const size_t len = strlen(cmdline);
  return load_addr > ((uint64_t)cmdline + (uint64_t)len);
}

bool check_mod(const uint64_t load_addr, const Mod *mod) {
  // check the module data
  if (load_addr <= (uint64_t)(get_mod_end(mod))) {
    return false;
  }

  // check the Mod structure
  if (load_addr <= ((uint64_t)mod + (uint64_t)sizeof(Mod))) {
    return false;
  }

  // check the Mod string
  const char *s = get_mod_string(mod);
  const size_t len = strlen(s);
  return load_addr > ((uint64_t)s + (uint64_t)len);
}

bool check_mods(const uint64_t load_addr, const MIS *mis) {
  const Mod *mods = get_mods(mis);
  for (size_t i = 1; i < mis->mods_count; ++i) {
    if (!check_mod(load_addr, &mods[i])) {
      return false;
    }
  }
  return true;
}

// check_all runs all of the address checking functions to ensure that
// the target load address is above all of the structures loaded by
// GRUB. If this returns true, then the load address is safe and will
// not clobber any of GRUB's or the module loader's structures.
bool check_all(const uint64_t load_addr, const MIS *mis) {
  return check_mods(load_addr, mis) && check_MIS(load_addr, mis) && check_cmdline(load_addr, mis);
}
