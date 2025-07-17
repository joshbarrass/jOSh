#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

uintmax_t strtoany(const char *restrict s, char **restrict endptr, int base,
                   uintmax_t max_val, bool is_signed) {
  *endptr = s;
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

  // short circuit now if the string contains no numerical chars
  if (*s < '0' || *s > '9') {
    return 0;
  }

  uintmax_t n = 0;
  uintmax_t n_prev = 0;
  bool overflowed = false;
  while (*s >= '0' && *s <= '9') {
    // C99 standard says sign is applied to the parsed value at the
    // end, rather than applying it now, so the issue around
    // INT_MAX/INT_MIN should be an expected edge case as part of the
    // standard
    n = 10*n + (*s - '0');

    // detect overflow
    if (n > max_val || n < n_prev) {
      overflowed = true;
    }
    ++s;
  }

  *endptr = s;
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
