#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include <stdint.h>
#include <stddef.h>

#ifdef MB2_TYPES
typedef enum mb2_type : uint16_t {
    MB2_TYPE_NULL = 0,
    MB2_TYPE_INFO_REQ = 1,
    MB2_TYPE_ADDRESS = 2,
    MB2_TYPE_ENTRY = 3,
    MB2_TYPE_ENTRY_EFI_X86 = 8,
    MB2_TYPE_ENTRY_EFI_AMD64 = 9,
    MB2_TYPE_FLAGS = 4,
    MB2_TYPE_FRAMEBUFFER = 5,
    MB2_TYPE_MODULE_ALIGN = 6,
    MB2_TYPE_EFI_SERVICES = 7,
    MB2_TYPE_RELOCATABLE = 10,
} mb2_type;
#endif

#define N_M2IS_TYPES 22
typedef enum m2is_type : uint32_t {
    M2IS_TYPE_NULL = 0,
    M2IS_TYPE_CMDLINE = 1,
    M2IS_TYPE_BOOTLOADER_NAME = 2,
    M2IS_TYPE_MODULE = 3,
    M2IS_TYPE_MEMINFO = 4,
    M2IS_TYPE_BIOS_BOOT_DEV = 5,
    M2IS_TYPE_MEMMAP = 6,
    M2IS_TYPE_MEMMAP_EFI = 17,
    M2IS_TYPE_VBE_INFO = 7,
    M2IS_TYPE_FRAMEBUFFER = 8,
    M2IS_TYPE_ELF_SYMBOLS = 9,
    M2IS_TYPE_APM_TABLE = 10,
    M2IS_TYPE_EFI32_SYS_TABLE = 11,
    M2IS_TYPE_EFI64_SYS_TABLE = 12,
    M2IS_TYPE_SMBIOS_TABLE = 13,
    M2IS_TYPE_ACPI1_RSDP = 14,
    M2IS_TYPE_ACPI2_RSDP = 15,
    M2IS_TYPE_DHCP = 16,
    M2IS_TYPE_EFI_SERVICES = 18,
    M2IS_TYPE_EFI32_IMAGE = 19,
    M2IS_TYPE_EFI64_IMAGE = 20,
    M2IS_TYPE_LOAD_ADDR = 21,
} m2is_type;

#ifndef MB2_ENUMS_ONLY
typedef struct __attribute__((packed)) m2is_tag {
  m2is_type type;
  uint32_t size;
} m2is_tag;

typedef struct __attribute__((packed)) M2IS {
  uint32_t size;
  uint32_t reserved;
  const m2is_tag tags;
} M2IS;

typedef struct {
  const void *tags_start;
  const uint32_t length;
  size_t offset;
} m2is_tag_iterator;

static m2is_tag_iterator new_m2is_iterator(const M2IS *M2IS) {
  m2is_tag_iterator iter = {
    .tags_start = &M2IS->tags,
    .length = M2IS->size - ((uintptr_t)&(M2IS->tags) - (uintptr_t)M2IS),
    .offset = 0 };
  return iter;
}

static const m2is_tag *m2is_iterator_next(m2is_tag_iterator *iter) {
  const m2is_tag *tag = (m2is_tag*)(iter->tags_start + iter->offset);
  if (iter->offset >= iter->length || tag->type == M2IS_TYPE_NULL) return NULL;
  iter->offset += tag->size;
  if (iter->offset % 8 != 0) {
    iter->offset += 8 - (iter->offset % 8);
  }
  return tag;
}

typedef struct __attribute__((packed)) m2is_module {
  m2is_tag tag;
  uint32_t mod_start;
  uint32_t mod_end;
  char string[0];
} m2is_module;

typedef struct __attribute__((packed)) m2is_meminfo {
  m2is_tag tag;
  uint32_t mem_lower;
  uint32_t mem_upper;
} m2is_meminfo;

#define MULTIBOOT2_MMAP_TYPE_FREE 1
#define MULTIBOOT2_MMAP_TYPE_ACPI 3
#define MULTIBOOT2_MMAP_TYPE_HIBERNATION 4
#define MULTIBOOT2_MMAP_TYPE_BAD 5

typedef struct __attribute__((packed)) mmap_entry {
  uint64_t base_addr;
  uint64_t length;
  uint32_t type;
  uint32_t __reserved;
} mmap_entry;

typedef struct __attribute__((packed)) m2is_mmap {
  m2is_tag tag;
  uint32_t entry_size;
  uint32_t entry_version;
  mmap_entry entries[0];
} m2is_mmap;

typedef struct __attribute__((packed)) m2is_framebuffer_info {
  m2is_tag tag;
  uint64_t addr;
  uint32_t pitch;
  uint32_t width;
  uint32_t height;
  uint8_t bpp;
  uint8_t type;
  uint8_t __reserved;
  uint8_t color_info[];
} m2is_framebuffer_info;

typedef struct __attribute__((packed)) {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} m2is_palette_entry;

typedef struct __attribute__((packed)) {
  uint32_t num_colors;
  m2is_palette_entry palette[];
} m2is_color_info_indexed;

typedef struct __attribute__((packed)) {
  uint8_t red_offset;
  uint8_t red_bits;
  uint8_t green_offset;
  uint8_t green_bits;
  uint8_t blue_offset;
  uint8_t blue_bits;
} m2is_color_info_direct;

#endif

#endif
