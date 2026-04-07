#ifndef __BOOTSTRAP_DISPLAY_H
#define __BOOTSTRAP_DISPLAY_H

#include <kernel/drivers/console.h>
#include <kernel/bootstruct.h>
#include <multiboot2.h>

ConsoleDriver *bootstrap_console_driver_m2is(const M2IS *m2is);
ConsoleDriver *bootstrap_console_driver_bootstruct(const BootStruct *fbinfo);

#endif
