#include <string.h>

void *memmove(void *dest, const void *src, size_t n) {
  const char *s = (char*)src;
  char *d = (char*)dest;

  if (s > d) {
    // data starts in front of the destination
    // copy from start to end to avoid overwriting
    for (size_t i = 0; i < n; ++i) {
      d[i] = s[i];
    }
  } else {
    // data starts behind the destination
    // copy from end to start
    for (size_t i = n; i > 0; --i) {
      d[i-1] = s[i-1];
    }
  }
  return dest;
}
