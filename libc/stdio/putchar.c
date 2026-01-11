#include <stdio.h>

#ifdef __is_libk
#include <kernel/tty.h>

int putchar(char c) {
  ConsoleDriver *drv = get_kernel_console_driver();
  return drv->putchar(drv, c);
}

#endif
