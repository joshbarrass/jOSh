#include <kernel/tty.h>
#include <kernel/drivers/console.h>

static ConsoleDriver console_driver;

ConsoleDriver *get_kernel_console_driver() {
  return &console_driver;
}

void set_kernel_console_driver(ConsoleDriver drv) {
  console_driver = drv;
}

void term_set_color(const CharColor color) {
  return console_driver.set_color(&console_driver, color);
}

void term_set_fg(const int fg) {
  return console_driver.set_fg(&console_driver, fg);
}

void term_set_bg(const int bg) {
  return console_driver.set_bg(&console_driver, bg);
}

void term_clear() {
  return console_driver.clear(&console_driver);
}

void term_draw_bitmap(const ScreenChar *bitmap, const size_t x, const size_t y,
                      const size_t w, const size_t h) {
  return console_driver.draw_bitmap(&console_driver, bitmap, x, y, w, h);
}
