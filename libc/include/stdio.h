#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>

int putchar(char c);
int puts(char *s);
int vprintf(const char *fmt, va_list args);
int printf(const char *fmt, ...);

#endif
