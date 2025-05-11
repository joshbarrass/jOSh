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

void clear_screen_color(const CharColor color);
void clear_screen_bgfg(const int bg, const int fg);
void clear_screen();
void print_string(const char *s, const int x, const int y);

#endif
