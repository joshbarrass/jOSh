#include <stdarg.h>
#include <kernel/panic.h>
#include <kernel/tty.h>
#include <stdio.h>

#include <kernel/panic_image.h>

__attribute__((noreturn)) __attribute__((naked)) static void kpanic_halt() {
  asm volatile (
                ".hang:\r\n"
                "cli\r\n"
                "hlt\r\n"
                "jmp .hang\r\n"
                );
}

static void term_draw_bitmap(const ScreenChar *bitmap, const size_t x, const size_t y,
                 const size_t w, const size_t h) {
  struct Terminal *term = get_default_term();

  for (size_t i = 0; i < h; ++i) {
    for (size_t j = 0; j < w; ++j) {
      term->drv->put_char_at(term->drv, bitmap[w*i+j], x+j, y+i);
    }
  }
}

void kpanic(const char* fmt, ...){
  term_set_fg(VGA_COLOR_WHITE);
  term_set_bg(VGA_COLOR_RED);
  term_clear();
  term_draw_bitmap(skull, VGA_WIDTH-SKULL_WIDTH, 0, SKULL_WIDTH, SKULL_HEIGHT);
  printf("KERNEL PANIC\n\n");

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);

  kpanic_halt();
}
