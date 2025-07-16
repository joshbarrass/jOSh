#ifndef PANIC_H
#define PANIC_H 1

#ifdef __cplusplus
extern "C" {
#endif

void kpanic(const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
