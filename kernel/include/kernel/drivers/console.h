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

typedef struct __attribute__((packed)) ConsoleDriver {
  void (*put_char_at)(struct ConsoleDriver *drv, const ScreenChar c, const size_t x, const size_t y);
  void (*line_feed)(struct ConsoleDriver *drv, const CharColor color);
  void (*clear)(struct ConsoleDriver *drv, const CharColor color);
  size_t width;
  size_t height;
} ConsoleDriver;

#endif
