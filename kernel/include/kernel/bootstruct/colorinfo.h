#ifndef __BOOTSTRUCT_COLORINFO_H
#define __BOOTSTRUCT_COLORINFO_H

#include <kernel/bootstruct/types.h>

typedef struct __attribute__((packed)) {
  uint8_t red_offset;
  uint8_t red_bits;
  uint8_t green_offset;
  uint8_t green_bits;
  uint8_t blue_offset;
  uint8_t blue_bits;
} color_info_direct;

typedef struct __attribute__((packed)) {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} color_info_palette_entry;

typedef struct __attribute__((packed)) {
  uint32_t num_colors;
  bs_ptr_t palette;
} color_info_indexed;

#endif
