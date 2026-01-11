#ifndef __DRIVER_EGA_H
#define __DRIVER_EGA_H

#include <kernel/drivers/console.h>

ConsoleDriver ega_driver_init(void * const framebuffer, const size_t width, const size_t height,
                              const size_t pitch);

#endif
