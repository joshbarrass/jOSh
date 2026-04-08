#ifndef __BOOTSTRUCT_FRAMEBUFFER_H
#define __BOOTSTRUCT_FRAMEBUFFER_H

#include <kernel/bootstruct/types.h>
#include <kernel/bootstruct/colorinfo.h>
#include <multiboot2.h>

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
    color_info_direct direct;
    color_info_indexed indexed;
  } color_info;
} BootStruct_fbinfo;

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

#endif
