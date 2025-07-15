#include <stdio.h>

#ifdef __is_libk
#include <kernel/tty.h>

int puts(char *s) {
  int i = 0;
  for (; s[i] != '\0'; ++i) {
    putchar(s[i]);
  }
  return i;
}

#endif
