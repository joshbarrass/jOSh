#include <kernel/drivers/ega/ega.h>
#include <kernel/tty.h>

static inline ScreenChar *ega_get_row(EGAConsole *console, const size_t y) {
  return (ScreenChar*)((char*)console->framebuffer + y*console->pitch);
}

static void ega_clear_color(ConsoleDriver *drv, const CharColor color) {
  EGAConsole *console = (EGAConsole*)drv;
  for (size_t y = 0; y < console->drv.height; ++y) {
    ScreenChar *row = ega_get_row(console, y);
    for (size_t x = 0; x < console->drv.width; ++x) {
      row[x].character = 0;
      row[x].color = color;
    }
  }
}

static void ega_scroll(ConsoleDriver *drv, const CharColor color) {
  EGAConsole *console = (EGAConsole*)drv;
  for (size_t y = 1; y < console->drv.height; ++y) {
    ScreenChar *row = ega_get_row(console, y);
    ScreenChar *row_above = ega_get_row(console, y-1);
    for (size_t x = 0; x < console->drv.width; ++x) {
      row_above[x] = row[x];
    }
  }
  ScreenChar *row = ega_get_row(console, console->drv.height-1);
  for (size_t x = 0; x < console->drv.width; ++x) {
    row[x].character = 0;
    row[x].color = color;
  }
}

static void ega_putchar_at(ConsoleDriver *drv, const ScreenChar c, const size_t x, const size_t y) {
  EGAConsole *console = (EGAConsole*)drv;
  ScreenChar *row = ega_get_row(console, y);
  row[x] = c;
}

void ega_driver_init(EGAConsole *drv, void * const framebuffer, const size_t width,
                     const size_t height, const size_t pitch) {
  drv->drv.put_char_at = &ega_putchar_at;
  drv->drv.line_feed = &ega_scroll;
  drv->drv.clear = &ega_clear_color;
  drv->drv.width = width;
  drv->drv.height = height;
  drv->pitch = pitch;
  drv->framebuffer = framebuffer;
}
