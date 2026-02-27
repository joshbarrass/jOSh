#include <kernel/drivers/null_console/null_console.h>
#include <kernel/drivers/console.h>

static bool console_init = false;
static ConsoleDriver null_console;

static void null_putch(struct ConsoleDriver *drv, const ScreenChar c,
                        const size_t x, const size_t y) {}
static void null_line_feed(struct ConsoleDriver *drv, const CharColor color) {}
static void null_clear(struct ConsoleDriver *drv, const CharColor color) {}

ConsoleDriver *get_null_console() {
  if (!console_init) {
    null_console.put_char_at = &null_putch;
    null_console.line_feed = &null_line_feed;
    null_console.clear = &null_clear;
    null_console.width = 1;
    null_console.height = 1;
  }
  return &null_console;
}

bool is_null_console(ConsoleDriver *drv) {
  return drv == &null_console;
}
