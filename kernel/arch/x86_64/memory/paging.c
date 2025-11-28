#include <kernel/x86_64/memory/paging.h>

struct ptindices virt_addr_to_ptindices(uintptr_t addr) {
  struct ptindices out;

  addr >>= 12;
  out.pt_i = addr & 511;
  addr >>= 9;
  out.pd_i = addr & 511;
  addr >>= 9;
  out.pdpt_i = addr & 511;
  addr >>= 9;
  out.pml4t_i = addr & 511;

  return out;
}
