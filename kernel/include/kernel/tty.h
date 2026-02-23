#ifndef TTY_H
#define TTY_H 1

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <kernel/vgadef.h>
#include <kernel/drivers/console.h>

#ifdef __cplusplus
extern "C" {
#endif

  struct Terminal {
    ConsoleDriver *drv;
    size_t pos_x;
    size_t pos_y;
    CharColor color;
  };

  void init_default_term();
  struct Terminal *get_default_term();
  void set_default_console_driver(ConsoleDriver *drv);

  void term_set_color(const CharColor color);
  void term_set_fg(const int fg);
  void term_set_bg(const int bg);
  void term_clear();
  int term_putchar(const char c);
  int term_puts(const char *s);

#ifdef __cplusplus
}
#endif

#endif
