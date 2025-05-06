#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"
#include "addr_checker.h"

// check_all runs all of the address checking functions to ensure that
// the target load address is above all of the structures loaded by
// GRUB. If this returns true, then the load address is safe and will
// not clobber any of GRUB's or the module loader's structures.
bool check_all(uint64_t load_addr) {
  return false;
}
