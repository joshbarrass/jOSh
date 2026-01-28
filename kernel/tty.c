#include <kernel/tty.h>
#include <kernel/drivers/console.h>

static ConsoleDriver default_driver;
static struct Terminal default_term;

void init_default_term() {
  default_term.drv = &default_driver;
  default_term.pos_x = 0;
  default_term.pos_y = 0;
  default_term.color.fg = VGA_COLOR_LIGHT_GREY;
  default_term.color.bg = VGA_COLOR_BLACK;
}

void set_default_console_driver(ConsoleDriver drv) {
  default_driver = drv;
}

struct Terminal *get_default_term() {
  return &default_term;
}

static void term_new_line() {
  default_term.pos_x = 0;
  ++default_term.pos_y;
  if (default_term.pos_y == default_term.drv->height) {
    default_term.drv->line_feed(default_term.drv, default_term.color);
    --default_term.pos_y;
  }
}

static void term_carriage_return() {
  default_term.pos_x = 0;
}

int term_putchar(const char c) {
  switch (c) {
  case '\n':
    term_new_line(); return c;
  case '\r':
    term_carriage_return(); return c;
  case '\f':
    term_clear(); return c;
  }

  const ScreenChar sc = {.character=c, .color=default_term.color};
  default_term.drv->put_char_at(default_term.drv, sc, default_term.pos_x, default_term.pos_y);

  ++default_term.pos_x;
  if (default_term.pos_x == default_term.drv->width) {
    default_term.pos_x = 0;
    ++default_term.pos_y;
  }
  
  if (default_term.pos_y == default_term.drv->height) {
    default_term.drv->line_feed(default_term.drv, default_term.color);
    --default_term.pos_y;
  }
  return (int)c;
}

int term_puts(const char *s) {
  int n = 0;
  while (*s != 0) {
    term_putchar(*(s++));
    ++n;
  }
  return n;
}

void term_set_color(const CharColor color) {
  default_term.color = color;
}

void term_set_fg(const int fg) {
  default_term.color.fg = fg;
}

void term_set_bg(const int bg) {
  default_term.color.bg = bg;
}

void term_clear() {
  default_term.drv->clear(default_term.drv, default_term.color);
  default_term.pos_x = 0;
  default_term.pos_y = 0;
}

/* void term_draw_bitmap(const ScreenChar *bitmap, const size_t x, const size_t y, */
/*                       const size_t w, const size_t h) { */
/*   return console_driver.draw_bitmap(&console_driver, bitmap, x, y, w, h); */
/* } */

