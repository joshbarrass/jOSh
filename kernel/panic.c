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

void kpanic(const char* fmt, ...){
  term_set_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
  term_clear_screen();
  draw_bitmap(skull, VGA_WIDTH-SKULL_WIDTH, 0, SKULL_WIDTH, SKULL_HEIGHT);
  term_println("KERNEL PANIC");
  term_new_line();

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);

  kpanic_halt();
}
