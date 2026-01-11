#include <stdint.h>

#define MULTIBOOT_MAGIC 0xE85250D6
#define MULTIBOOT_ARCH_X86 0

struct __attribute__ ((packed)) multiboot_magic {
  uint32_t magic;
  uint32_t architecture;
  uint32_t length;
  uint32_t checksum;
};

struct __attribute__((packed)) multiboot_tag {
  uint16_t type;
  uint16_t flags;
  uint32_t size;
};

#define DEFAULTS struct multiboot_tag start
#define TAG(T, TYPE, FLAGS) .start = { .type = TYPE, .flags = FLAGS, .size = sizeof(T) }

struct __attribute__((packed)) end_tag {
  DEFAULTS;
};
#define END_TAG { TAG(struct end_tag, 0, 0) }

struct __attribute__((packed)) multiboot_module_alignment {
  DEFAULTS;
};
#define MODULE_ALIGN_TAG                                                       \
  { TAG(struct multiboot_module_alignment, 6, 0) }

struct __attribute__((packed, aligned (8))) multiboot_header {
  struct multiboot_magic magic;
  struct multiboot_module_alignment align;
  struct end_tag end;
};

__attribute__((section(".multiboot"), aligned(8))) const struct multiboot_header header = {
    .magic =
        {
            .magic = MULTIBOOT_MAGIC,
            .architecture = MULTIBOOT_ARCH_X86,
            .length = sizeof(struct multiboot_header),
            .checksum = -(MULTIBOOT_MAGIC + MULTIBOOT_ARCH_X86 + sizeof(struct multiboot_header)),
        },
    .align = MODULE_ALIGN_TAG,
    .end = END_TAG,
};
