#ifndef __DRIVERS_BITMAP_CONSOLE_H
#define __DRIVERS_BITMAP_CONSOLE_H

#include <kernel/drivers/console.h>
#include <kernel/bootstruct/colorinfo.h>

typedef struct __attribute__((packed)) {
  ConsoleDriver drv;
  size_t pitch;
  uint8_t *addr;
  size_t width_px;
  size_t height_px;
  uint8_t bpp;
  uint8_t phys_bpp;
  color_info_direct color_info;

  // for direct colour, 32-bit indexing is most common. Since we're
  // using a fixed palette, we can pre-generate and store the colour
  // dwords as an optimisation.
  uint32_t palette_32bpp[16];
} BitmapConsole;

void bitmap_console_init(BitmapConsole *, const void* addr, const size_t width, const size_t height, const size_t pitch, const uint8_t bpp, const color_info_direct color_info);

#endif
