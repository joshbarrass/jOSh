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
  ConsoleDriver *drv = get_kernel_console_driver();
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
