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

typedef enum uint8_t {
  BS_FB_TYPE_UNKNOWN,
  BS_FB_TYPE_INDEXED,
  BS_FB_TYPE_DIRECT,
  BS_FB_TYPE_TEXT,
} bs_fb_type_t;

typedef struct __attribute__((packed)) {
  bs_ptr_t phys_addr;
  bs_ptr_t virt_addr;
  bs_fb_type_t type;
  uint32_t width;
  uint32_t height;
  uint32_t pitch;
  uint8_t bpp;
  union {
    m2is_color_info_indexed indexed;
    m2is_color_info_direct direct;
  } color_info;
} BootStruct_fbinfo;

typedef struct __attribute__((packed)) {
  bs_flags_t flags;
  bs_flags_t checksum;

  bs_ptr_t M2IS;
  bs_ptr_t lowest_free_addr;
  BootStruct_fbinfo fbinfo;

} BootStruct;

inline static const M2IS *bs_get_MIS(const BootStruct *bs) {
  return (const M2IS *)((uintptr_t)bs->M2IS);
}

inline static phys_addr_t bs_get_lowest_free_addr(const BootStruct *bs) {
  return (phys_addr_t)bs->lowest_free_addr;
}

inline static bs_fb_type_t m2is_fb_type_to_bs(const uint8_t type) {
  switch (type) {
  case M2IS_FB_TYPE_TEXT:
    return BS_FB_TYPE_TEXT;
  case M2IS_FB_TYPE_DIRECT:
    return BS_FB_TYPE_DIRECT;
  case M2IS_FB_TYPE_INDEXED:
    return BS_FB_TYPE_INDEXED;
  }
  return BS_FB_TYPE_UNKNOWN;
}

void bs_init(BootStruct *bs);
bool bs_verify_checksum(const BootStruct *bs);
void bs_set_checksum(BootStruct *bs);
bool bs_convert_fbinfo(BootStruct *bs, const M2IS *m2is);


#endif
