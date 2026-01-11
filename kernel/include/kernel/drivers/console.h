#ifndef __CONSOLE_DRIVER_H
#define __CONSOLE_DRIVER_H

#include <kernel/tty.h>
#include <stddef.h>

typedef struct ConsoleDriver {
  int (*putchar)(struct ConsoleDriver *drv, const char c);
  int (*puts)(struct ConsoleDriver *drv, const char *s);
  void (*draw_bitmap)(struct ConsoleDriver *drv, const ScreenChar *bitmap, const size_t x, const size_t y,
                     const size_t w, const size_t h);
  ScreenChar *framebuffer;
  size_t width;
  size_t height;
  size_t pitch;
  CharColor terminal_color;
  size_t x;
  size_t y;
} ConsoleDriver;

#endif
