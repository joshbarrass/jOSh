#ifndef __DRIVERS_BITMAP_CONSOLE_H
#define __DRIVERS_BITMAP_CONSOLE_H

#include <kernel/drivers/console.h>
#include <multiboot2.h>

typedef struct __attribute__((packed)) {
  ConsoleDriver drv;
  size_t pitch;
  uint8_t *addr;
  size_t width_px;
  size_t height_px;
  uint8_t bpp;
  m2is_color_info_direct color_info;
} BitmapConsole;

void bitmap_console_init(BitmapConsole *, m2is_framebuffer_info *);

#endif
