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

  ConsoleDriver *get_kernel_console_driver();
  void set_kernel_console_driver(ConsoleDriver drv);

#ifdef __cplusplus
}
#endif

#endif
