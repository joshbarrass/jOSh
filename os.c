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

#include "tty.h"
#include "module_loader/multiboot.h"

static char *stack[16*1024] __attribute__((section(".bss"))); // 16-bit stack

static const char *welcomeMessage = "Welcome to jOSh! "
#ifdef ARCH_64
  "(64-bit)";
#elif ARCH_32
  "(32-bit)";
#else
;
#endif

const MIS *mis = NULL;

void kernel_main();

__attribute__((optimize("O0"))) void _entry() {
  // bind our registers to variable names to ensure they don't get
  // ruined
  register volatile uint32_t rax asm("eax");
  register volatile MIS *rbx asm("ebx");

  // set up a new stack, since the loader stack may not be sufficient
  // (and if it is, we've probably clobbered it loading the kernel ELF
  // anyway), then call the kernel
#ifdef ARCH_64
  register volatile char *rsp asm("rsp")
#elif ARCH_32
  register volatile char *rsp asm("esp")
#endif
    = stack + sizeof(stack)/sizeof(stack[0]);

  // if we were booted by a multiboot, we can save the pointer to the
  // MIS
  if ((uint32_t)rax == (uint32_t)0x2BADB002) {
    mis = (MIS*)rbx;
  }
  kernel_main();

  // create a footer to hang if the kernel ever returns here
  asm volatile (
                ".hang:\r\n"
                "cli\r\n"
                "hlt\r\n"
                "jmp .hang\r\n"
                );
}

void print_hex(uint64_t v, const size_t x, const size_t y, char length) {
  v &= ~0ULL >> (64 - length * 8);
  char buf[2+2*8+1];
  for (size_t i = 0; i < sizeof(buf) / sizeof(buf[0]); ++i) {
    buf[i] = '0';
  }
  buf[2+2*length] = 0;
  buf[1] = 'x';
  size_t i = 1+2*length;
  do {
    uint64_t r = v % 16;
    v /= 16;
    if (r < 10) {
      buf[i] = r + '0';
    } else if (r >= 10) {
      buf[i] = (r-10) + 'A';
    }
    --i;
  } while ( v > 0 );
  print_string(buf, x, y);
  return;
}

void kernel_main() {
  clear_screen();
  print_string(welcomeMessage, 0, 0);
  if (mis == NULL) {
    terminal_color.fg = 4;
    print_string("Missing multiboot information structure", 0, 1);
  }
  print_string("[*] Checking required multiboot flags...", 0, 1);
  terminal_color.fg = 10;
  if (mis->FLAGS & MULTIBOOT_FLAG_FULL_MMAP) {
    print_string("[+] Memory map is available", 0, 2);
  } else {
    terminal_color.fg = 4;
    print_string("[!] Memory map unavailable", 0, 2);
    return;
  }
  // ensure the memory map has not been corrupted
  // if below 2MB, none of our loaded data could have collided with it
  if (mis->mmap >= 2 * 1024 * 1024 || mis->mmap + mis->mmap_length >= 2 * 1024 * 1024) { 
    terminal_color.fg = 4;
    print_string("[!] Memory map corrupted", 0, 3);
    return;
  } else {
    print_string("[+] Intact mmap found at 0x????????????????", 0, 3);
    print_hex(mis->mmap, 25, 3, 8);
  }
  print_string("    |       start       -         end       |type| raw|", 0, 4);
  size_t y = 5;
  terminal_color.fg = 7;
  size_t adv = 0;
  mmap *map = get_mmap(mis);
  for (size_t i = 0; i < mis->mmap_length; i+=adv) {
    mmap *entry = (mmap*)(&((char*)map)[i]);
    adv = entry->size+4;
    print_string("|0x???????????????? - 0x????????????????|    |    |", 4, y);
    print_hex(entry->base_addr, 5, y, 8);
    print_hex(entry->base_addr+entry->length-1, 5+18+3, y, 8);
    switch (entry->type) {
    case 1:
      print_string("FREE", 45, y);
      break;
    case 2:
      print_string("RSVD", 45, y);
      break;
    case 3:
      print_string("ACPI", 45, y);
      break;
    case 4:
      print_string("RESH", 45, y);
      break;
    case 5:
      terminal_color.fg = 4;
      print_string("BAD!", 45, y);
      terminal_color.fg = 7;
      break;
    default:
      print_string("RSVD", 45, y);
      print_hex(entry->type, 45+5, y, 1);
      break;
    }
    ++y;
  }
  return;
}
