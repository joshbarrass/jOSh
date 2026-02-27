#include <stdio.h>

#ifdef __is_libk
#include <kernel/tty.h>

int puts(char *s) {
  return term_puts(s);
}

#endif
