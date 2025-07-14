#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

// defines a safe buffer size for formatting an integer to a string in
// denary form
#define PRINT_INT_BUFFER_SIZE(type) ((8*sizeof(type))*30103/100000+5)

// safe buffer size for formatting an integer to a string in hex
// form. This is much simpler, since each hex digit is 4 bits
#define PRINT_HEX_BUFFER_SIZE(type) (2*sizeof(type) + 1)

#ifdef __is_libk
#include <kernel/tty.h>

static void print_uint(unsigned int d, const bool negative) {
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

static void print_int(const int d) {
  print_uint((d < 0) ? (unsigned int)(~d) + 1u : d, (d < 0) ? true : false);
}

static void print_hex_uint(unsigned int v, const bool uppercase) {
  const size_t buflen = PRINT_HEX_BUFFER_SIZE(int);
  char buf[buflen];

  size_t i = 0;
  do {
    const char r = v % 16;
    v = v / 16;
    buf[i] = (r < 10 ? '0' : (uppercase ? ('A'-10) : ('a'-10))) + r;
    ++i;
  } while (v != 0 && i < buflen);

  // now go through the buffer in reverse to print the chars
  // this loop will stop at i=0, including printing i=0
  while (i-->0) {
    term_print_char(buf[i]);
  }
}

int vprintf(const char *fmt, va_list args) {
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
        print_int(va_arg(args, int));
        break;
      case 'u':
        print_uint(va_arg(args, unsigned int), false);
        break;
      case 'x':
        print_hex_uint(va_arg(args, unsigned int), false);
        break;
      case 'X':
        print_hex_uint(va_arg(args, unsigned int), true);
        break;
      default:
        term_print_char('%');
      case '%':
        term_print_char(*fmt);
      }
    }
  }
  return 0; // TODO:
}

#endif
