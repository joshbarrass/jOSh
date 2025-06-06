#ifndef TTY_H
#define TTY_H 1

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct CharColor {
  unsigned char fg : 4;
  unsigned char bg : 4;
};

struct ScreenChar {
  unsigned char character;
  struct CharColor color;
};
volatile struct ScreenChar * const screen = (struct ScreenChar*)0xB8000;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
struct CharColor terminal_color = {7, 0};

void clear_screen_color(const struct CharColor color) {
  for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; ++i) {
    screen[i].character = 0;
    screen[i].color = color;
  }
}

void clear_screen_bgfg(const int bg, const int fg) {
  const struct CharColor color = {fg, bg};
  clear_screen_color(color);
}

void clear_screen() {
  clear_screen_color(terminal_color);
}

void print_string(const char *s, const int x, const int y) {
  const size_t pos = VGA_WIDTH*y+x;
  for (size_t i = 0; s[i] != 0 && (struct ScreenChar*)(pos+i) < screen+VGA_WIDTH*VGA_HEIGHT; ++i) {
    screen[pos+i].character = s[i];
    screen[pos+i].color.fg = terminal_color.fg;
    screen[pos+i].color.bg = terminal_color.bg;
  }
}

#endif
