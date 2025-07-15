#ifndef TTY_H
#define TTY_H 1

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <kernel/vgadef.h>

typedef struct {
  unsigned char fg : 4;
  unsigned char bg : 4;
} CharColor;

typedef struct {
  unsigned char character;
  CharColor color;
} ScreenChar;

size_t term_get_pos_x();
size_t term_get_pos_y();
void term_set_color(const unsigned char fg, const unsigned char bg);
void term_set_fg(const unsigned char fg);
void term_set_bg(const unsigned char bg);
void term_clear_screen_color(const CharColor color);
void term_clear_screen_bgfg(const int bg, const int fg);
void term_clear_screen();
size_t term_print_char_at(const char c, const int x, const int y);
void term_print_char(const char c);
size_t term_print_string_at(const char *s, const int x, const int y);
void term_print_string(const char *s);
void term_println(const char *s);
void term_scroll();
void term_scroll_n(const unsigned char n);
void term_new_line();
void draw_bitmap(const ScreenChar *bitmap, const size_t x, const size_t y, const size_t w, const size_t h);

#endif
