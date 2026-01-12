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

  ConsoleDriver *get_kernel_console_driver();
  void set_kernel_console_driver(ConsoleDriver drv);

  void term_set_color(const CharColor color);
  void term_set_fg(const int fg);
  void term_set_bg(const int bg);
  void term_clear();
  void term_draw_bitmap(const ScreenChar *bitmap, const size_t x, const size_t y,
                        const size_t w, const size_t h);

#ifdef __cplusplus
}
#endif

#endif
