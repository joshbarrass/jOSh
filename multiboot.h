#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct Mod {
  void *mod_start;
  void *mod_end;
  char *string;
  uint32_t _reserved;
} Mod;

typedef struct MIS {
  uint32_t FLAGS;

  uint32_t mem_lower;
  uint32_t mem_upper;

  uint32_t boot_device;

  char *cmdline;

  uint32_t mods_count;
  Mod *mods;
  
  // TODO: rest of the header
} MIS;

#endif
