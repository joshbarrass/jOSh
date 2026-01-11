#include <kernel/tty.h>
#include <kernel/drivers/console.h>

static ConsoleDriver console_driver;

ConsoleDriver *get_kernel_console_driver() {
  return &console_driver;
}

void set_kernel_console_driver(ConsoleDriver drv) {
  console_driver = drv;
}
