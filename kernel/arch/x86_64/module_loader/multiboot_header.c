#include <stdint.h>
#define MB2_TYPES
#define MB2_ENUMS_ONLY
#include <multiboot2.h>

#define MULTIBOOT_MAGIC 0xE85250D6
#define MULTIBOOT_ARCH_X86 0

#define MULTIBOOT_FLAG_REQUIRE_CONSOLE (1 << 0)
#define MULTIBOOT_FLAG_EGA (1 << 1)

struct __attribute__ ((packed)) multiboot_magic {
  uint32_t magic;
  uint32_t architecture;
  uint32_t length;
  uint32_t checksum;
};

struct __attribute__((packed)) multiboot_tag {
  mb2_type type;
  uint16_t flags;
  uint32_t size;
};

#define DEFAULTS struct multiboot_tag start
#define TAG(T, TYPE, FLAGS) .start = { .type = TYPE, .flags = FLAGS, .size = sizeof(T) }

// null/end tag

struct __attribute__((packed)) end_tag {
  DEFAULTS;
};
#define END_TAG { TAG(struct end_tag, 0, 0) }

// module alignment tag

struct __attribute__((packed)) multiboot_module_alignment {
  DEFAULTS;
};
#define MODULE_ALIGN_TAG                                                       \
  { TAG(struct multiboot_module_alignment, MB2_TYPE_MODULE_ALIGN, 0) }

// info request tag

struct __attribute__((packed)) multiboot_info_request {
  DEFAULTS;
  // We use the number of available types as a safe default. This
  // allows us to request every available type if we want to, and the
  // remaining values will be zero-initialised, which should be safe
  // (since this corresponds to the null type an will be ignored)
  uint32_t types[N_M2IS_TYPES];
  // fix the alignment. Odd array length -> ALIGN(4). Even array length -> ALIGN(8)
  #if (N_M2IS_TYPES % 2 != 0)
  uint32_t __reserved;
  #endif
};

// flags tag
struct __attribute__((packed)) multiboot_flags {
  DEFAULTS;
  uint32_t console_flags;
  uint32_t __reserved;
};

struct __attribute__((packed, aligned(8))) multiboot_header {
  struct multiboot_magic magic;
  struct multiboot_module_alignment align;
  struct multiboot_info_request info;
  struct multiboot_flags flags;
  struct end_tag end;
};

__attribute__((section(".multiboot"), aligned(8)))
const struct multiboot_header header = {
    .magic =
        {
            .magic = MULTIBOOT_MAGIC,
            .architecture = MULTIBOOT_ARCH_X86,
            .length = sizeof(struct multiboot_header),
            .checksum = -(MULTIBOOT_MAGIC + MULTIBOOT_ARCH_X86 +
                          sizeof(struct multiboot_header)),
        },
    .end = END_TAG,
    .align = MODULE_ALIGN_TAG,
    .info =
        {
            TAG(struct multiboot_info_request, MB2_TYPE_INFO_REQ, 0),
            .types = {M2IS_TYPE_CMDLINE, M2IS_TYPE_MODULE, M2IS_TYPE_MEMINFO,
                      M2IS_TYPE_MEMMAP},
        },
    .flags = {TAG(struct multiboot_flags, MB2_TYPE_FLAGS, 0),
              .console_flags = MULTIBOOT_FLAG_REQUIRE_CONSOLE | MULTIBOOT_FLAG_EGA,
    },
};
