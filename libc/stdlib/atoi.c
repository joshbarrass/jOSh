#include <stdlib.h>
#include <ctype.h>

long long int atoll(const char *s) {
  // need to parse up to the first invalid character

  // skip leading whitespace
  while (isspace(*s)) {
    ++s;
  }

  // parse the sign (if present)
  long long int sign = 1;
  if (s[0] == '-') {
    sign = -1;
    // shift the start of the string, rather than adding some offset
    ++s;
  } else if (s[0] == '+') {
    ++s;
  }

  long long int n = 0;
  while (*s >= '0' && *s <= '9') {
    // we apply the sign to each digit, instead of at the end, to
    // avoid the potential overflow issues around LLONG_MAX/LLONG_MIN
    // i.e. |LLONG_MIN| = LLONG_MAX+1
    n = 10*n + sign*(*s - '0');
    ++s;
  }

  return n;
}

long int atol(const char *s) {
  return (long int)atoll(s);
}

int atoi(const char *s) {
  return (int)atoll(s);
}
