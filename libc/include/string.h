#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memmove(void*, const void*, size_t);
size_t strlen(const char *str);
int strcmp(const char *s1, const char *s2);

#ifdef __cplusplus
}
#endif

#endif
