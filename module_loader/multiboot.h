#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#if defined(__x86_64__) || defined(_M_X64)
#define ARCH_64 1
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#define ARCH_32 1
#endif

#ifdef ARCH_32
typedef struct __attribute__((packed)) {
  void *mod_start;
  void *mod_end;
  char *string;
  uint32_t _reserved;
} Mod;
inline const void *get_mod_start(const Mod *m) { return m->mod_start; }
inline const void *get_mod_end(const Mod *m) { return m->mod_end; }
inline const char *get_mod_string(const Mod *m) { return m->string; }
#else
typedef struct __attribute__((packed)) {
  uint32_t mod_start;
  uint32_t mod_end;
  uint32_t string;
  uint32_t _reserved;
} Mod;
inline const void *get_mod_start(const Mod *m) { return (const void*)(m->mod_start); }
inline const void *get_mod_end(const Mod *m) { return (const void*)(m->mod_end); }
inline const char *get_mod_string(const Mod *m) { return (const char*)(m->string); }
#endif

typedef struct __attribute__((packed)) {
  uint32_t size;
  uint64_t base_addr;
  uint64_t length;
  uint32_t type;
} mmap;

typedef struct __attribute__((packed)) {
  uint32_t FLAGS;

  uint32_t mem_lower;
  uint32_t mem_upper;

  uint32_t boot_device;

  #ifdef ARCH_32
  char *cmdline;
  #else
  uint32_t cmdline;
  #endif

  uint32_t mods_count;
  #ifdef ARCH_32
  Mod *mods;
  #else
  uint32_t mods;
  #endif

  uint32_t syms_0;
  uint32_t syms_1;
  uint32_t syms_2;
  uint32_t syms_3;

  uint32_t mmap_length;
  #ifdef ARCH_32
  mmap *mmap;
  #else
  uint32_t mmap;
  #endif
  
  // TODO: rest of the header
} MIS;
#ifdef ARCH_32
inline const char *get_cmdline(const MIS *m) { return m->cmdline; }
inline const Mod *get_mods(const MIS *m) { return m->mods; }
inline const mmap *get_mmap(const MIS *m) { return m->mmap; }
#else
inline const char *get_cmdline(const MIS *m) { return (const char*)(m->cmdline); }
inline const Mod *get_mods(const MIS *m) { return (const Mod*)(m->mods); }
inline const mmap *get_mmap(const MIS *m) { return (const mmap *)(m->mmap); }
#endif

#define MULTIBOOT_FLAG_4K_ALIGN (1 << 0)
#define MULTIBOOT_FLAG_BASIC_MEM_INFO (1 << 1)
#define MULTIBOOT_FLAG_VIDEO_INFO (1 << 2)
#define MULTIBOOT_FLAG_MODS (1 << 3)
#define MULTIBOOT_FLAG_A_OUT_SYMS (1 << 4)
#define MULTIBOOT_FLAG_ELF_SYMS (1 << 5)
#define MULTIBOOT_FLAG_FULL_MMAP (1 << 6)
#define MULTIBOOT_FLAG_DRIVES (1 << 7)
#define MULTIBOOT_FLAG_CONFIG_TBL (1 << 8)
#define MULTIBOOT_FLAG_BOOTLOADER_NAME (1 << 9)
#define MULTIBOOT_FLAG_APM (1 << 10)
#define MULTIBOOT_FLAG_VBE (1 << 11)
#define MULTIBOOT_FLAG_FRAMEBUFFER (1 << 12)
#define MULTIBOOT_FLAG_CUSTOM_LOAD (1 << 16)

#endif
