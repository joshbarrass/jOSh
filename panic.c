#include "panic.h"
#include "terminal/tty.h"

#include "panic_image.h"

void kpanic(){
  term_set_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
  term_clear_screen();
  draw_bitmap(skull, VGA_WIDTH-SKULL_WIDTH, 0, SKULL_WIDTH, SKULL_HEIGHT);
  term_println("KERNEL PANIC");
  term_new_line();
}
