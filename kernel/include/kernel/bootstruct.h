#ifndef __BOOTSTRUCT_H
#define __BOOTSTRUCT_H

#include <stdint.h>
#include <stdbool.h>
#include <multiboot2.h>
#include <kernel/bootstruct_flags.h>
#include <kernel/memory/types.h>

#define BOOTSTRUCT_MAGIC (0x1B002ED1)

typedef uint32_t bs_flags_t;
typedef uint64_t bs_ptr_t;

typedef struct __attribute__((packed)) {
  bs_flags_t flags;
  bs_flags_t checksum;

  bs_ptr_t M2IS;
  bs_ptr_t lowest_free_addr;
  bs_ptr_t fb_virt_addr;
  
} BootStruct;

inline static const M2IS *bs_get_MIS(const BootStruct *bs) {
  return (const M2IS *)((uintptr_t)bs->M2IS);
}

inline static phys_addr_t bs_get_lowest_free_addr(const BootStruct *bs) {
  return (phys_addr_t)bs->lowest_free_addr;
}

void bs_init(BootStruct *bs);
bool bs_verify_checksum(const BootStruct *bs);
void bs_set_checksum(BootStruct *bs);

#endif
