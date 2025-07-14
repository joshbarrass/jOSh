#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_64 1
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#define ARCH_32 1
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include <kernel/tty.h>
#include <multiboot.h>
#include <kernel/panic.h>

static const char *welcomeMessage = "Welcome to jOSh! "
#ifdef ARCH_64
  "(64-bit)";
#elif ARCH_32
  "(32-bit)";
#else
;
#endif

const MIS *mis = NULL;

void kernel_main() {
  term_clear_screen();
  term_println(welcomeMessage);
  if (mis != NULL) {
    term_println(get_mod_string(&get_mods(mis)[0]));
    term_new_line();
  }
  kpanic("This is a debug panic.\nFile: %s:%d\n\nTest1: %dTest2: %d\nTest3: %dTest4: %d\nTest5: %dTest6: %d\nTest7: %uTest8: %dTest 9: %x\nTest 10: %X", __FILE__, __LINE__, 123LL, -123, 987654321, -987654321, INT_MAX, INT_MIN, UINT_MAX, 0, 0xDEADBEEF, 0xFEDC1234);
  return;
}
