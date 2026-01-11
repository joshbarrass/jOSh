#include <stdio.h>

#ifdef __is_libk
#include <kernel/tty.h>

int puts(char *s) {
  ConsoleDriver *drv = get_kernel_console_driver();
  return drv->puts(drv, s);
}

#endif
