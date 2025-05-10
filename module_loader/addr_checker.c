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

uint64_t check_MIS(const MIS *mis) {
  return ((uint64_t)mis + (uint64_t)sizeof(MIS));
}

uint64_t check_cmdline(const MIS *mis) {
  const char *cmdline = get_cmdline(mis);
  const size_t len = strlen(cmdline);
  return ((uint64_t)cmdline + (uint64_t)len);
}

uint64_t check_mod(const Mod *mod) {
  uint64_t max_addr = 0;

  // check the module data
  uint64_t mod_end = (uint64_t)(get_mod_end(mod));
  if (mod_end > max_addr) {
    max_addr = mod_end;
  }

  // check the Mod structure
  uint64_t ModS_end = ((uint64_t)mod + (uint64_t)sizeof(Mod));
  if (ModS_end > max_addr) {
    max_addr = ModS_end;
  }

  // check the Mod string
  const char *s = get_mod_string(mod);
  const size_t len = strlen(s);
  uint64_t str_end = ((uint64_t)s + (uint64_t)len);
  if (str_end > max_addr) {
    max_addr = str_end;
  }
  return max_addr;
}

uint64_t check_mods(const MIS *mis) {
  const Mod *mods = get_mods(mis);
  uint64_t max_addr = 0;
  for (size_t i = 1; i < mis->mods_count; ++i) {
    uint64_t mod_max = check_mod(&mods[i]);
    if (mod_max > max_addr) {
      max_addr = mod_max;
    }
  }
  return max_addr;
}

// get_MIS_max_addr runs all of the functions for determining the max
// addresses and returns the greatest of them.
uint64_t get_MIS_max_addr(const MIS *mis) {
  uint64_t max_addr = 0;
  const uint64_t mods = check_mods(mis);
  if (mods > max_addr) {
    max_addr = mods;
  }
  const uint64_t mis_addr = check_MIS(mis);
  if (mis_addr > max_addr) {
    max_addr = mis_addr;
  }
  const uint64_t cmdline = check_cmdline(mis);
  if (cmdline > max_addr) {
    max_addr = cmdline;
  }
  return max_addr;
}

// gets the maximum address used by the MIS/GRUB and checks whether a
// given address is greater than it. If this function returns true, it
// is safe to relocate data to this address, and this will not
// overwrite any other data loaded by GRUB.
bool check_all(const uint64_t load_addr, const MIS *mis) {
  return load_addr > get_MIS_max_addr(mis);
}
