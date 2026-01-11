#include <stdio.h>

#ifdef __is_libk
#include <kernel/tty.h>

int putcs(char *s) {
  ConsoleDriver *drv = get_kernel_console_driver();
  drv->puts(drv, s);
}

#endif
