#ifndef __CONSOLE_DRIVER_H
#define __CONSOLE_DRIVER_H

#include <stddef.h>

typedef struct {
  unsigned char fg : 4;
  unsigned char bg : 4;
} CharColor;

typedef struct {
  unsigned char character;
  CharColor color;
} ScreenChar;

typedef struct ConsoleDriver {
  int (*putchar)(struct ConsoleDriver *drv, const char c);
  int (*puts)(struct ConsoleDriver *drv, const char *s);
  void (*draw_bitmap)(struct ConsoleDriver *drv, const ScreenChar *bitmap, const size_t x, const size_t y,
                     const size_t w, const size_t h);
  void (*set_color)(struct ConsoleDriver *drv, const CharColor color);
  void (*set_bg)(struct ConsoleDriver *drv, const int bg);
  void (*set_fg)(struct ConsoleDriver *drv, const int fg);
  void (*clear)(struct ConsoleDriver *drv);
  ScreenChar *framebuffer;
  size_t width;
  size_t height;
  size_t pitch;
  CharColor terminal_color;
  size_t x;
  size_t y;
} ConsoleDriver;

#endif
