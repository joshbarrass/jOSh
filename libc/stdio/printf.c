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

static int print_uint(unsigned int d, const bool negative) {
  const size_t buflen = PRINT_INT_BUFFER_SIZE(unsigned int);
  char buf[buflen];

  size_t i = 0;
  do {
    const char r = d % 10;
    d = d / 10;
    buf[i] = '0'+r;
    ++i;
  } while (d != 0 && i < buflen);
  int written = 0;
  if (negative) {
    putchar('-');
    ++written;
  }

  // now go through the buffer in reverse to print the chars
  // this loop will stop at i=0, including printing i=0
  while (i-->0) {
    putchar(buf[i]);
    ++written;
  }
  return written;
}

static int print_int(const int d) {
  return print_uint((d < 0) ? (unsigned int)(~d) + 1u : d, (d < 0) ? true : false);
}

static int print_hex_uint(unsigned int v, const bool uppercase) {
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
  int written = 0;
  while (i-->0) {
    putchar(buf[i]);
    ++written;
  }
  return written;
}

int vprintf(const char *fmt, va_list args) {
  int written = 0;
  
  for (; *fmt != 0; ++fmt) {
    if (*fmt != '%') {
      putchar(*fmt);
      ++written;
    } else if (*fmt == '%') {
      ++fmt;
      
      switch (*fmt) {
      case 's':
        written += puts(va_arg(args, char*));
        break;
      case 'i':
      case 'd':
        written += print_int(va_arg(args, int));
        break;
      case 'u':
        written += print_uint(va_arg(args, unsigned int), false);
        break;
      case 'x':
        written += print_hex_uint(va_arg(args, unsigned int), false);
        break;
      case 'X':
        written += print_hex_uint(va_arg(args, unsigned int), true);
        break;
      default:
        putchar('%');
        ++written;
      case '%':
        putchar(*fmt);
        ++written;
      }
    }
  }
  return written;
}

int printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  return vprintf(fmt, args);
}
