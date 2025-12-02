#include <stdio.h>
#include <stdio_ops.h>

// wraps the putchar function provided by stdio.h
static int putchar_wrapper(struct io_ops *ctx, char c) {
  return putchar(c);
}

// wraps the puts function provided by stdio.h
static int puts_wrapper(struct io_ops *ctx, char *s) {
  return puts(s);
}

// returns an ops struct for the stdio functions
struct io_ops get_stdio_ops() {
  const struct io_ops to_return = { &putchar_wrapper, &puts_wrapper };
  return to_return;
}

static int putchar_buffer(struct io_ops *ctx, char c) {
  struct buffer_ops *parent = (struct buffer_ops*)ctx;
  *(parent->buf++) = c;
  return (int)c;
}

static int puts_buffer(struct io_ops *ctx, char *s) {
  struct buffer_ops *parent = (struct buffer_ops*)ctx;
  int i = 0;
  for (; s[i] != '\0'; ++i) {
    *(parent->buf++) = s[i];
  }
  return i;
}

struct buffer_ops get_buffer_ops(char *buf) {
  struct buffer_ops ctx = {{&putchar_buffer, &puts_buffer}, buf};
  return ctx;
}
