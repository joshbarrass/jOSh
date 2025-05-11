#include "tty.h"

volatile ScreenChar * const screen = (ScreenChar*)VGA_FRAMEBUFFER_ADDR;
static CharColor terminal_color = {VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK};

void clear_screen_color(const CharColor color) {
  for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; ++i) {
    screen[i].character = 0;
    screen[i].color = color;
  }
}

void clear_screen_bgfg(const int bg, const int fg) {
  const CharColor color = {fg, bg};
  clear_screen_color(color);
}

void clear_screen() {
  clear_screen_color(terminal_color);
}

void print_string(const char *s, const int x, const int y) {
  const size_t pos = VGA_WIDTH*y+x;
  for (size_t i = 0; s[i] != 0 && (ScreenChar*)(pos+i) < screen+VGA_WIDTH*VGA_HEIGHT; ++i) {
    screen[pos+i].character = s[i];
    screen[pos+i].color.fg = terminal_color.fg;
    screen[pos+i].color.bg = terminal_color.bg;
  }
}
