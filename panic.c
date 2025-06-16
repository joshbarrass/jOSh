#include <stdarg.h>
#include "panic.h"
#include "terminal/tty.h"

#include "panic_image.h"

// defines a safe buffer size for formatting an integer to a string in
// denary form
#define PRINT_INT_BUFFER_SIZE(type) (8*sizeof(type))*30103/100000+5

static void kpanic_print_uint(unsigned int d, const bool negative) {
  const size_t buflen = PRINT_INT_BUFFER_SIZE(unsigned int);
  char buf[buflen];

  size_t i = 0;
  do {
    const char r = d % 10;
    d = d / 10;
    buf[i] = '0'+r;
    ++i;
  } while (d != 0 && i < buflen);
  if (negative) {
    term_print_char('-');
  }

  // now go through the buffer in reverse to print the chars
  // this loop will stop at i=0, including printing i=0
  while (i-->0) {
    term_print_char(buf[i]);
  }
}

static void kpanic_print_int(const int d) {
  kpanic_print_uint((d < 0) ? ~d + 1ULL : d, (d < 0) ? true : false);
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
      case 'i':
      case 'd':
        kpanic_print_int(va_arg(args, int));
        break;
      case 'u':
        kpanic_print_uint(va_arg(args, unsigned int), false);
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
