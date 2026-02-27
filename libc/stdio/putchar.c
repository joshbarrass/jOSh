#include <stdio.h>

#ifdef __is_libk
#include <kernel/tty.h>

int putchar(char c) {
  return term_putchar(c);
}

#endif
