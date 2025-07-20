#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

static inline bool check_digit(const char c, int base) {
  if (base > 36) {
    base = 36;
  } else if (base < 2) {
    base = 2;
  }
  const bool numeric = (c >= '0' && c <= (base >= 10 ? '9' : '0' + base - 1));
  if (base <= 10) {
    return numeric;
  }
  const int base_shift = base-11;
  return numeric || (c >= 'A' && c <= ('A'+base_shift)) || (c >= 'a' && c <= 'a'+base_shift);
}

// we can use char return type because the digit will always be in the range 0-35
static int parse_digit(const char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
  if (c >= 'a' && c <= 'z') return c - 'a' + 10;
  return -1;
}

static uintmax_t strtoany(const char *restrict s, char **restrict endptr, int base,
                   uintmax_t max_val, bool is_signed) {
  *endptr = (char *restrict)s;
  // skip leading whitespace
  while (isspace(*s)) {
    ++s;
  }

  // parse the sign (if present)
  bool negative = false;
  if (s[0] == '-') {
    negative = true;
    ++s;
  } else if (s[0] == '+') {
    ++s;
  }

  if (!negative && is_signed) {
    // the maximum positive value we can store is one less than the
    // maximum absolute value for a signed type
    --max_val;
  }

  // determine the base automatically if necessary
  if (base == 0) {
    if (s[0] == '0') {
      if (s[1] == 'x' || s[1] == 'X') {
        base = 16;
        s += 2;
      } else {
        base = 8;
        s += 1;
      }
    } else {
      base = 10;
    }
  }

  // short circuit now if the string contains no numerical chars
  if (!check_digit(*s, base)) {
    return 0;
  }

  uintmax_t n = 0;
  bool overflowed = false;
  while (check_digit(*s, base)) {
    // C99 standard says sign is applied to the parsed value at the
    // end, rather than applying it now, so the issue around
    // INT_MAX/INT_MIN should be an expected edge case as part of the
    // standard

    // we can skip doing any actual work if we've already overflowed;
    // it doesn't make a difference
    if (!overflowed) {
      uintmax_t digit = parse_digit(*s);
      // n = base*n + digit
      // use compiler built-ins to efficiently detect overflow
      if (__builtin_mul_overflow(n, base, &n) ||
          __builtin_add_overflow(n, digit, &n) || n > max_val) {
        overflowed = true;
      }
    }
    ++s;
  }

  *endptr = (char *restrict)s;
  if (overflowed) {
    errno = ERANGE;
    return max_val;
  }
  // we didn't overflow, so follow the C standard regarding applying
  // the sign
  if (negative) {
    return -(uintmax_t)n;
  }
  return n;
}

uintmax_t strtoumax(const char *restrict s, char **restrict endptr, int base) {
  return strtoany(s, endptr, base, UINTMAX_MAX, false);
}

intmax_t strtoimax(const char *restrict s, char **restrict endptr, int base) {
  return strtoany(s, endptr, base, INTMAX_MIN, true);
}

unsigned long long int strtoull(const char *restrict s, char **restrict endptr, int base) {
  return strtoany(s, endptr, base, ULLONG_MAX, false);
}

long long int strtoll(const char *restrict s, char **restrict endptr, int base) {
  return strtoany(s, endptr, base, LLONG_MIN, true);
}

unsigned long int strtoul(const char *restrict s, char **restrict endptr, int base) {
  return strtoany(s, endptr, base, ULONG_MAX, false);
}

long int strtol(const char *restrict s, char **restrict endptr, int base) {
  return strtoany(s, endptr, base, LONG_MIN, true);
}

