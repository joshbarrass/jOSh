#ifndef _STDLIB_H
#define _STDLIB_H

#include <stdint.h>

int atoi(const char *s);
long int atol(const char *s);
long long int atoll(const char *s);

int abs(int x);
long labs(long x);
long long llabs(long long x);

uintmax_t strtoumax(const char * restrict s, char ** restrict endptr, int base);
intmax_t strtoimax(const char * restrict s, char ** restrict endptr, int base);
unsigned long long int strtoull(const char *restrict s, char **restrict endptr, int base);
long long int strtoll(const char *restrict s, char **restrict endptr, int base);
unsigned long int strtoul(const char *restrict s, char **restrict endptr, int base);
long int strtol(const char *restrict s, char **restrict endptr, int base);

#endif
