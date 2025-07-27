#ifndef __ELF_PAGED_H
#define __ELF_PAGED_H

#include <stdint.h>
#include "elf.h"

int elf64_map_program_image(const char *const, uint64_t *const);

#endif
