#ifndef __DRIVER_NULL_CONSOLE_H
#define __DRIVER_NULL_CONSOLE_H

#include <stdbool.h>
#include <kernel/drivers/console.h>

ConsoleDriver *get_null_console();
bool is_null_console(ConsoleDriver*);

#endif
