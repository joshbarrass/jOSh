#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <archdef.h>

#ifdef ARCH_32
typedef struct __attribute__((packed)) {
  void *mod_start;
  void *mod_end;
  char *string;
  uint32_t _reserved;
} Mod;
static inline const void *get_mod_start(const Mod *m) { return m->mod_start; }
static inline const void *get_mod_end(const Mod *m) { return m->mod_end; }
static inline const char *get_mod_string(const Mod *m) { return m->string; }
#else
typedef struct __attribute__((packed)) {
  uint32_t mod_start;
  uint32_t mod_end;
  uint32_t string;
  uint32_t _reserved;
} Mod;
static inline const void *get_mod_start(const Mod *m) { return (const void*)(m->mod_start); }
static inline const void *get_mod_end(const Mod *m) { return (const void*)(m->mod_end); }
static inline const char *get_mod_string(const Mod *m) { return (const char*)(m->string); }
#endif

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
  
  // TODO: rest of the header
} MIS;
#ifdef ARCH_32
static inline const char *get_cmdline(const MIS *m) { return m->cmdline; }
static inline const Mod *get_mods(const MIS *m) { return m->mods; }
#else
static inline const char *get_cmdline(const MIS *m) { return (const char*)(m->cmdline); }
static inline const Mod *get_mods(const MIS *m) { return (const Mod*)(m->mods); }
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
