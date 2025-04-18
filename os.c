#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

struct ScreenChar {
    unsigned char character;
    unsigned char fg : 4;
    unsigned char bg : 4;
};
volatile struct ScreenChar * const screen = (struct ScreenChar*)0xB8000;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void clear_screen(const char bg, const char fg) {
  for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; ++i) {
    screen[i].character = 0;
    screen[i].fg = fg;
    screen[i].bg = bg;
  }
}

void print_string(const char *s, const int x, const int y) {
  for (int i = 0; s[i] != 0; ++i) {
    screen[i+VGA_WIDTH*y+x].character = s[i];
  }
}

static const char *welcomeMessage = "Welcome to jOSh!";

void kernel_main() {
  clear_screen(0x0, 0x7);
  screen->character = 'P';
  print_string(welcomeMessage, 0, 0);
  
  return;
}
