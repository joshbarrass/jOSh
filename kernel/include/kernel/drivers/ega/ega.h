#ifndef __DRIVER_EGA_H
#define __DRIVER_EGA_H

#include <kernel/drivers/console.h>

// shadows ConsoleDriver to permit passing the pointer to anything
// that needs a ConsoleDriver
typedef struct __attribute__((packed)) {
  ConsoleDriver drv;
  size_t pitch;
  void *framebuffer;
} EGAConsole;

void ega_driver_init(EGAConsole *drv, void * const framebuffer, const size_t width,
                     const size_t height, const size_t pitch);

#endif
