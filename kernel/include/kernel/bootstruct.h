#ifndef __BOOTSTRUCT_H
#define __BOOTSTRUCT_H

#include <stdint.h>
#include <stdbool.h>
#include <multiboot.h>
#include <kernel/bootstruct_flags.h>

#define BOOTSTRUCT_MAGIC (0x1B002ED1)

typedef uint32_t bs_flags_t;
typedef uint64_t bs_ptr_t;

typedef struct __attribute__((packed)) {
  bs_flags_t flags;
  bs_flags_t checksum;

  bs_ptr_t MIS;
  
} BootStruct;

inline static const MIS *bs_get_MIS(const BootStruct *bs) {
  return (const MIS *)((uintptr_t)bs->MIS);
}

void bs_init(BootStruct *bs);
bool bs_verify_checksum(const BootStruct *bs);
void bs_set_checksum(BootStruct *bs);

#endif
