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
inline void *get_mod_start(Mod *m) { return m->mod_start; }
inline void *get_mod_end(Mod *m) { return m->mod_end; }
inline char *get_mod_string(Mod *m) { return m->string; }
#else
typedef struct __attribute__((packed)) {
  uint32_t mod_start;
  uint32_t mod_end;
  uint32_t string;
  uint32_t _reserved;
} Mod;
inline void *get_mod_start(Mod *m) { return (void*)(m->mod_start); }
inline void *get_mod_end(Mod *m) { return (void*)(m->mod_end); }
inline char *get_mod_string(Mod *m) { return (char*)(m->string); }
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
inline char *get_cmdline(MIS *m) { return m->cmdline; }
inline Mod *get_mods(MIS *m) { return m->mods; }
#else
inline char *get_cmdline(MIS *m) { return (char*)(m->cmdline); }
inline Mod *get_mods(MIS *m) { return (Mod*)(m->mods); }
#endif

#endif
