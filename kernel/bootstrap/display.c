#include <kernel/bootstrap/display.h>
#include <kernel/bootstrap/static_bump.h>
#include <kernel/drivers/console.h>
#include <kernel/drivers/ega/ega.h>
#include <kernel/vgadef.h>

static _Alignas(uint64_t) uint8_t buf[256];
static StaticBumper bumper = STATIC_BUMP_ALLOCATOR(buf);

// TODO: What arguments should this function want? The M2IS?
ConsoleDriver *bootstrap_console_driver() {
  EGAConsole * const drv = SB_ALLOC_ALIGNED(&bumper, EGAConsole);
  // TODO: figure this out from the bootloader information
  ega_driver_init(drv, (void*)VGA_FRAMEBUFFER_ADDR, 80, 25, 160);
  return (ConsoleDriver*)drv;
}
