#ifndef TTY_H
#define TTY_H 1

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <kernel/vgadef.h>
#include <kernel/drivers/console.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  unsigned char fg : 4;
  unsigned char bg : 4;
} CharColor;

typedef struct {
  unsigned char character;
  CharColor color;
} ScreenChar;

  ConsoleDriver *get_kernel_console_driver();
  void set_kernel_console_driver(ConsoleDriver drv);

#ifdef __cplusplus
}
#endif

#endif
