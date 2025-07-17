#ifndef _STDLIB_H
#define _STDLIB_H

#include <stdint.h>

int atoi(const char *s);
long int atol(const char *s);
long long int atoll(const char *s);

uintmax_t strtoumax(const char * restrict s, char ** restrict endptr, int base);
intmax_t strtoimax(const char * restrict s, char ** restrict endptr, int base);

#endif
