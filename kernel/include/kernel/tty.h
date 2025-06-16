#ifndef TTY_H
#define TTY_H 1

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  unsigned char fg : 4;
  unsigned char bg : 4;
} CharColor;

typedef struct {
  unsigned char character;
  CharColor color;
} ScreenChar;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_FRAMEBUFFER_ADDR 0xB8000

#define VGA_COLOR_BLACK 0
#define VGA_COLOR_BLUE 1
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_CYAN 3
#define VGA_COLOR_RED 4
#define VGA_COLOR_MAGENTA 5
#define VGA_COLOR_BROWN 6
#define VGA_COLOR_LIGHT_GREY 7
#define VGA_COLOR_DARK_GREY 8
#define VGA_COLOR_LIGHT_BLUE 9
#define VGA_COLOR_LIGHT_GREEN 10
#define VGA_COLOR_LIGHT_CYAN 11
#define VGA_COLOR_LIGHT_RED 12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_LIGHT_BROWN 14
#define VGA_COLOR_WHITE 15

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
