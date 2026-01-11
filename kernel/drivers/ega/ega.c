#include <kernel/drivers/ega/ega.h>
#include <kernel/tty.h>

static size_t ega_get_pos_x(ConsoleDriver *console) {
  return console->x;
}

static size_t ega_get_pos_y(ConsoleDriver *console) {
  return console->y;
}

static void ega_set_color(ConsoleDriver *console, const unsigned char fg, const unsigned char bg) {
  console->terminal_color.fg = fg;
  console->terminal_color.bg = bg;
}

static void ega_set_fg(ConsoleDriver *console, const unsigned char fg) {
  console->terminal_color.fg = fg;
}

static void ega_set_bg(ConsoleDriver *console, const unsigned char bg) {
  console->terminal_color.bg = bg;
}

static ScreenChar *ega_get_row(ConsoleDriver *console, const size_t y) {
  return (ScreenChar*)((char*)console->framebuffer + y*console->pitch);
}

static void ega_clear_color(ConsoleDriver *console, const CharColor color) {
  for (size_t y = 0; y < console->height; ++y) {
    ScreenChar *row = ega_get_row(console, y);
    for (size_t x = 0; x < console->width; ++x) {
      row[x].character = 0;
      row[x].color = color;
    }
  }
  console->x = 0;
  console->y = 0;
}

static void ega_clear_bgfg(ConsoleDriver *console, const int bg, const int fg) {
  const CharColor color = { fg, bg };
  ega_clear_color(console, color);
}

static void ega_clear(ConsoleDriver *console) {
  ega_clear_color(console, console->terminal_color);
}

static void ega_scroll(ConsoleDriver *console) {
  for (size_t y = 1; y < console->height; ++y) {
    ScreenChar *row = ega_get_row(console, y);
    ScreenChar *row_above = ega_get_row(console, y-1);
    for (size_t x = 0; x < console->width; ++x) {
      row_above[x] = row[x];
    }
  }
  ScreenChar *row = ega_get_row(console, console->height-1);
  for (size_t x = 0; x < console->width; ++x) {
    row[x].character = 0;
    row[x].color = console->terminal_color;
  }
}

static size_t ega_putchar_at(ConsoleDriver *console, const char c, const int x, const int y) {
  ScreenChar *row = ega_get_row(console, y);
  row[x].character = c;
  row[x].color.fg = console->terminal_color.fg;
  row[x].color.bg = console->terminal_color.bg;
  return console->width * y + x;
}

static void ega_new_line(ConsoleDriver *console) {
  console->x = 0;
  ++console->y;
  if (console->y == console->height) {
    term_scroll();
    --console->y;
  }
}

static void ega_carriage_return(ConsoleDriver *console) {
  console->x = 0;
}

static int ega_putchar(ConsoleDriver *console, const char c) {
  switch (c) {
  case '\n':
    ega_new_line(console); return c;
  case '\r':
    ega_carriage_return(console); return c;
  }
  
  const size_t new_pos = ega_putchar_at(console, c, console->x, console->y);
  console->x = new_pos / console->width;
  console->y = new_pos % console->width;
  while (console->y >= console->height) {
    ega_scroll(console);
    --console->y;
  }
  return (int)c;
}

static int ega_puts(ConsoleDriver *console, const char *s) {
  int n = 0;
  while (*s != 0) {
    ega_putchar(console, *(s++));
    ++n;
  }
  return n;
}

static void ega_draw_bitmap(ConsoleDriver *console, const ScreenChar *bitmap, const size_t x, const size_t y,
                 const size_t w, const size_t h) {
  for (size_t i = 0; i < h; ++i) {
    ScreenChar *row = ega_get_row(console, y+i);
    for (size_t j = 0; j < w; ++j) {
      row[x+j] = bitmap[w*i+j];
    }
  }
}

ConsoleDriver ega_driver_init(void * const framebuffer, const size_t width,
                                 const size_t height, const size_t pitch) {
  ConsoleDriver drv = {
    .putchar = &ega_putchar,
    .puts = &ega_puts,
    .draw_bitmap = &ega_draw_bitmap,
    .framebuffer = (ScreenChar *)framebuffer,
    .width = width,
    .height = height,
    .pitch = pitch,
    .terminal_color = {
      .fg = VGA_COLOR_LIGHT_GREY,
      .bg = VGA_COLOR_BLACK,
    },
    .x = 0,
    .y = 0,
  };
  return drv;
}
