#include "tty.h"

volatile ScreenChar * const screen = (ScreenChar*)VGA_FRAMEBUFFER_ADDR;
static CharColor terminal_color = {VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK};
static size_t pos_x = 0;
static size_t pos_y = 0;

size_t term_get_pos_x() {
  return pos_x;
}

size_t term_get_pos_y() {
  return pos_y;
}

void term_set_color(const unsigned char fg, const unsigned char bg) {
  terminal_color.fg = fg;
  terminal_color.bg = bg;
}

void term_set_fg(const unsigned char fg) {
  terminal_color.fg = fg;
}

void term_set_bg(const unsigned char bg) {
  terminal_color.bg = bg;
}

void term_clear_screen_color(const CharColor color) {
  for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; ++i) {
    screen[i].character = 0;
    screen[i].color = color;
  }
  pos_x = 0;
  pos_y = 0;
}

void term_clear_screen_bgfg(const int bg, const int fg) {
  const CharColor color = {fg, bg};
  term_clear_screen_color(color);
}

void term_clear_screen() {
  term_clear_screen_color(terminal_color);
}

size_t term_print_string_at(const char *s, const int x, const int y) {
  size_t pos = VGA_WIDTH*y+x;
  for (size_t i = 0; s[i] != 0; ++i) {
    while (pos >= VGA_WIDTH * VGA_HEIGHT) {
      pos -= VGA_WIDTH;
      term_scroll();
    }
    screen[pos].character = s[i];
    screen[pos].color.fg = terminal_color.fg;
    screen[pos].color.bg = terminal_color.bg;
    ++pos;
  }
  return pos;
}

void term_print_string(const char *s) {
  const size_t new_pos = term_print_string_at(s, pos_x, pos_y);
  pos_y = new_pos / VGA_WIDTH;
  pos_x = new_pos % VGA_WIDTH;
}

void term_println(const char *s) {
  term_print_string(s);
  term_new_line();
}

void term_scroll() {
  for (size_t row = 1; row < VGA_HEIGHT; ++row) {
    for (size_t col = 0; col < VGA_WIDTH; ++col) {
      screen[(row-1)*VGA_WIDTH+col] = screen[row*VGA_WIDTH+col];
    }
  }
  for (size_t col = 0; col < VGA_WIDTH; ++col) {
    screen[(VGA_HEIGHT-1)*VGA_WIDTH+col].character = 0;
    screen[(VGA_HEIGHT-1)*VGA_WIDTH+col].color = terminal_color;
  }
}

void term_scroll_n(const unsigned char n) {
  for (unsigned char i = 0; i < n; ++i) {
    term_scroll();
  }
}

void term_new_line() {
  pos_x = 0;
  ++pos_y;
  /* if (pos_y == VGA_HEIGHT) { */
  /*   term_scroll(); */
  /*   --pos_y; */
  /* } */
}

void draw_bitmap(const ScreenChar *bitmap, const size_t x, const size_t y,
                 const size_t w, const size_t h) {
  for (size_t i = 0; i < h; ++i) {
    for (size_t j = 0; j < w; ++j) {
      const size_t pos = VGA_WIDTH*(y+i) + x + j;
      screen[pos] = bitmap[w*i+j];
    }
  }
}
