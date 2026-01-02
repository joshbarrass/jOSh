#include <inttypes.h>

#define ABS_IMPL(T, name)                                               \
  T name(T x) {                                                             \
    T a = x >> (8 * sizeof(T) - 1);                                            \
    return (x ^ a) - a;                                                        \
  }

ABS_IMPL(int, abs);
ABS_IMPL(long, labs);
ABS_IMPL(long long, llabs);
ABS_IMPL(intmax_t, imaxabs);
