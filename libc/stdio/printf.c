#include <stdio.h>
#include <stdio_ops.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

// defines a safe buffer size for formatting an integer to a string in
// denary form
#define PRINT_INT_BUFFER_SIZE(type) ((8*sizeof(type))*30103/100000+5)

// safe buffer size for formatting an integer to a string in hex
// form. This is much simpler, since each hex digit is 4 bits
#define PRINT_HEX_BUFFER_SIZE(type) (2*sizeof(type) + 1)

// emum type for the length formatter codes
typedef enum {
  LEN_NONE,
  LEN_HH,
  LEN_H,
  LEN_L,
  LEN_LL,
  LEN_J,
  LEN_Z,
  LEN_T,
  LEN_BIG_L
} int_length_t;

// struct for encoding the flags
typedef struct {
  bool left;
  bool force_sign;
  bool space;
  bool hash;
  bool zero;
  long width;
} flags_t;

static inline flags_t make_unset_flags() {
  flags_t to_return = {false, false, false, false, false, 0};
  return to_return;
}

// macros for the horrible ternary operator mess used for generating
// the correct va_arg
#define INT_LEN_ARG(length, args) \
  ((length) == LEN_HH ? (signed char)va_arg(args, int) :        \
   (length) == LEN_H ? (short int)va_arg(args, int) :     \
   (length) == LEN_L ? va_arg(args, long int) : \
   (length) == LEN_LL ? va_arg(args, long long int) : \
   (length) == LEN_J ? va_arg(args, intmax_t) : \
   (length) == LEN_Z ? va_arg(args, ptrdiff_t) : \
   (length) == LEN_T ? va_arg(args, ptrdiff_t) : \
   va_arg(args, int))
#define UINT_LEN_ARG(length, args) \
  ((length) == LEN_HH ? (unsigned char)va_arg(args, unsigned int) :    \
   (length) == LEN_H ? (unsigned short int)va_arg(args, unsigned int) : \
   (length) == LEN_L ? va_arg(args, unsigned long int) : \
   (length) == LEN_LL ? va_arg(args, unsigned long long int) : \
   (length) == LEN_J ? va_arg(args, uintmax_t) : \
   (length) == LEN_Z ? va_arg(args, size_t) : \
   (length) == LEN_T ? va_arg(args, size_t) :     \
   va_arg(args, unsigned int))

static int print_uint(struct io_ops *ctx, uintmax_t d, const bool negative, const flags_t flags) {
  const size_t buflen = PRINT_INT_BUFFER_SIZE(uintmax_t);
  char buf[buflen];

  size_t i = 0;
  do {
    const char r = d % 10;
    d = d / 10;
    buf[i] = '0'+r;
    ++i;
  } while (d != 0 && i < buflen);
  int written = 0;
  const bool needs_sign = negative || flags.force_sign || flags.space;

  int to_pad = flags.width - i;
  if (needs_sign) {
    --to_pad;
  }
  // pad the width before the sign for spaces
  if (!flags.left && !flags.zero) {
    for (int j = 0; j < to_pad; ++j) {
      ctx->putchar(ctx, ' ');
      ++written;
    }
  }
  
  if (negative) {
    ctx->putchar(ctx, '-');
    ++written;
  } else if (flags.force_sign) {
    ctx->putchar(ctx, '+');
    ++written;
  } else if (flags.space) {
    ctx->putchar(ctx, ' ');
    ++written;
  }

  // pad after for zeros
  if (!flags.left && flags.zero) {
    for (int j = 0; j < to_pad; ++j) {
      ctx->putchar(ctx, '0');
      ++written;
    }
  }

  // now go through the buffer in reverse to print the chars
  // this loop will stop at i=0, including printing i=0
  while (i-->0) {
    ctx->putchar(ctx, buf[i]);
    ++written;
  }

  if (flags.left) {
    while (written < flags.width) {
      ctx->putchar(ctx, ' ');
      ++written;
    }
  }

  return written;
}

static int print_int(struct io_ops *ctx, const intmax_t d, const flags_t flags) {
  return print_uint(ctx, (d < 0) ? (uintmax_t)(~d) + 1u : d, (d < 0) ? true : false, flags);
}

static int print_hex_uint(struct io_ops *ctx, uintmax_t v, const bool uppercase, const flags_t flags) {
  const size_t buflen = PRINT_HEX_BUFFER_SIZE(uintmax_t);
  char buf[buflen];

  size_t i = 0;
  do {
    const char r = v % 16;
    v = v / 16;
    buf[i] = (r < 10 ? '0' : (uppercase ? ('A'-10) : ('a'-10))) + r;
    ++i;
  } while (v != 0 && i < buflen);

  int written = 0;
  int to_pad = flags.width - i;
  if (flags.hash) {
    to_pad -= 2;
  }
  // pad before 0x if padding with spaces
  if (!flags.left && !flags.zero) {
    while (to_pad-->0) {
      ctx->putchar(ctx, ' ');
      ++written;
    }
  }

  if (flags.hash) {
    ctx->putchar(ctx, '0');
    ctx->putchar(ctx, uppercase ? 'X' : 'x');
    written += 2;
  }

  // pad after for zeros
  if (!flags.left && flags.zero) {
    while (to_pad-->0) {
      ctx->putchar(ctx, '0');
      ++written;
    }
  }

  // now go through the buffer in reverse to print the chars
  // this loop will stop at i=0, including printing i=0
  while (i-->0) {
    ctx->putchar(ctx, buf[i]);
    ++written;
  }

  // write extra spaces if left-aligned
  if (flags.left) {
    while (written < flags.width) {
      ctx->putchar(ctx, ' ');
      ++written;
    }
  }

  return written;
}

static int print_string(struct io_ops *ctx, char *string, const flags_t flags) {
  int written = 0;

  // Pad with spaces first, but only if a width was actually specified
  // and the string is right-aligned. We don't want to waste time with
  // a strlen if it doesn't actually matter.
  if (flags.width > 0 && !flags.left) {
    const size_t len = strlen(string);
    if (len < flags.width) {
      size_t to_pad = flags.width - len;
      written += to_pad;
      while (to_pad-->0) {
        ctx->putchar(ctx, ' ');
      }
    }
  }

  written += ctx->puts(ctx, string);

  if (flags.left) {
    while (written < flags.width) {
      ctx->putchar(ctx, ' ');
      ++written;
    }
  }

  return written;
}

static int vprintf_backend(struct io_ops *ctx, const char *fmt, va_list args) {
  int written = 0;
  
  for (; *fmt != 0; ++fmt) {
    if (*fmt != '%') {
      ctx->putchar(ctx, *fmt);
      ++written;
    } else if (*fmt == '%') {
      // parse flags if they exist
      flags_t flags = make_unset_flags();
      while (true) {
        ++fmt;
        switch (*fmt) {
        case '-':
          flags.left = true;
          continue;
        case '+':
          flags.force_sign = true;
          continue;
        case ' ':
          flags.space = true;
          continue;
        case '#':
          flags.hash = true;
          continue;
        case '0':
          flags.zero = true;
          continue;
        }
        break;
      }

      // parse width, if it exists
      // first check for wildcard width
      if (*fmt == '*') {
        // read the arg
        int wildcard_width = va_arg(args, int);
        flags.width = wildcard_width >= 0 ? wildcard_width : 0;
        ++fmt;
      } else {
        // otherwise, parse the number
        int old_errno = errno; // save and restore errno -- printf shouldn't modify it
        const char *width_end = fmt;
        flags.width = strtol(fmt, (char **)&width_end, 10);
        errno = old_errno;
        fmt = width_end;
      }

      // find length specifier, if it exists
      int_length_t length_specifier;
      switch (*fmt) {
        // easy cases first: single characters
      case 'j':
        length_specifier = LEN_J;
        break;
      case 'z':
        length_specifier = LEN_Z;
        break;
      case 't':
        length_specifier = LEN_T;
        break;
      /* // L is only relevant for floats
      case 'L':
        length_specifier = LEN_BIG_L;
        break;
      */
      // cases potentially with multiple characters
      case 'h':
        if (*(fmt + 1) == 'h') {
          length_specifier = LEN_HH;
          ++fmt;
        } else {
          length_specifier = LEN_H;
        }
        break;
      case 'l':
        if (*(fmt + 1) == 'l') {
          length_specifier = LEN_LL;
          ++fmt;
        } else {
          length_specifier = LEN_L;
        }
        break;
      default:
        --fmt;
        length_specifier = LEN_NONE;
        break;
      }

      ++fmt;
      switch (*fmt) {
      case 'n':
        written += print_int(ctx, written, flags);
        break;
      case 'c':
        ++written;
        ctx->putchar(ctx, (char)va_arg(args, int));
        break;
      case 's':
        written += print_string(ctx, va_arg(args, char*), flags);
        break;
      case 'i':
      case 'd':
        written += print_int(ctx, INT_LEN_ARG(length_specifier, args), flags);
        break;
      case 'u':
        written += print_uint(ctx, UINT_LEN_ARG(length_specifier, args), false, flags);
        break;
      case 'x':
        written += print_hex_uint(ctx, UINT_LEN_ARG(length_specifier, args), false, flags);
        break;
      case 'X':
        written += print_hex_uint(ctx, UINT_LEN_ARG(length_specifier, args), true, flags);
        break;
      default:
        ctx->putchar(ctx, '%');
        ++written;
      case '%':
        ctx->putchar(ctx, *fmt);
        ++written;
      }
    }
  }
  return written;
}

int vprintf(const char *fmt, va_list args) {
  struct io_ops ops = get_stdio_ops();
  return vprintf_backend(&ops, fmt, args);
}

int printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  const int written = vprintf(fmt, args);
  va_end(args);
  return written;
}

int vsprintf(char *buf, const char *fmt, va_list args) {
  struct buffer_ops ops = get_buffer_ops(buf);
  const int to_return = vprintf_backend((struct io_ops*)(&ops), fmt, args);
  ops.ops.putchar((struct io_ops*)&ops, '\0');
  return to_return;
}

int sprintf(char *buf, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  const int written = vsprintf(buf, fmt, args);
  va_end(args);
  return written;
}
