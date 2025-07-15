#include <stdio.h>

#ifdef __is_libk
#include <kernel/tty.h>

int putchar(char c) {
  switch (c) {
  case '\n':
    term_new_line();
    break;
  default:
    term_print_char(c);
    break;
  }
  return (int)c;
}

#endif
