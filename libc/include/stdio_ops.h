#ifndef _STDIO_OPS_H
#define _STDIO_OPS_H

#ifdef __cplusplus
extern "C" {
#endif

struct io_ops {
  int (*putchar)(struct io_ops *ctx, char c);
  int (*puts)(struct io_ops *ctx, char *s);
};

struct buffer_ops {
  struct io_ops ops;
  char *buf;
};

struct io_ops get_stdio_ops();
struct buffer_ops get_buffer_ops(char *buf);

#ifdef __cplusplus
}
#endif

#endif
