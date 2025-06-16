#include <stdarg.h>
#include "panic.h"
#include "terminal/tty.h"

#include "panic_image.h"

// defines a safe buffer size for formatting an integer to a string in
// denary form
#define PRINT_INT_BUFFER_SIZE(type) (8*sizeof(type))*30103/100000+5

static void kpanic_print_int(int d) {
  const size_t buflen = PRINT_INT_BUFFER_SIZE(int);
  char buf[buflen];
  for (size_t i = 0; i < buflen; ++i) {
    buf[i] = 0;
  }
  // pre-seed the buffer with zero
  // we can short-circuit the formatting logic if it's already 0
  buf[0] = '0';

  const bool negative = (d < 0) ? true : false;
  d = (d < 0) ? -d : d;

  size_t i = 0;
  while (d != 0 && i < buflen) {
    const char r = d % 10;
    d = d / 10;
    buf[i] = '0'+r;
    ++i;
  }
  if (negative && i < buflen) {
    buf[i] = '-';
  }

  // now go through the buffer in reverse to print the chars
  // this loop will stop at i=0 but not print buf[0]
  i = buflen-1;
  while (i != 0) {
    if (buf[i] != 0) {
      term_print_char(buf[i]);
    }
    --i;
  }
  // print buf[0] manually
  // it will *always* contain a value, because an integer cannot have
  // no length (it will always be at least a '0')
  term_print_char(buf[0]);
}

static void kpanic_vprintf(const char *fmt, va_list args) {
  for (; *fmt != 0; ++fmt) {
    if (*fmt != '%') {
      switch (*fmt) {
      case '\n':
        term_new_line();
        break;
      default:
        term_print_char(*fmt);
      }
    } else if (*fmt == '%') {
      ++fmt;
      switch (*fmt) {
      case 's':
        term_print_string(va_arg(args, char*));
        break;
      case 'd':
        kpanic_print_int(va_arg(args, int));
        break;
      default:
        term_print_char('%');
        term_print_char(*fmt);
      }
    }
  }
}

void kpanic(const char* fmt, ...){
  term_set_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
  term_clear_screen();
  draw_bitmap(skull, VGA_WIDTH-SKULL_WIDTH, 0, SKULL_WIDTH, SKULL_HEIGHT);
  term_println("KERNEL PANIC");
  term_new_line();

  va_list args;
  va_start(args, fmt);
  kpanic_vprintf(fmt, args);
}
