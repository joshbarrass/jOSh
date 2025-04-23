#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tty.h"

static const char *welcomeMessage = "Welcome to jOSh! "
#if defined(__x86_64__) || defined(_M_X64)
  "(64-bit)";
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
  "(32-bit)";
#endif

void kernel_main() {
  clear_screen();
  screen->character = 'P';
  print_string(welcomeMessage, 0, 0);
  
  return;
}
