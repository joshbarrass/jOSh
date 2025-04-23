#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tty.h"

static const char *welcomeMessage = "Welcome to jOSh!";

void kernel_main() {
  clear_screen();
  screen->character = 'P';
  print_string(welcomeMessage, 0, 0);
  
  return;
}
