#ifndef ADDR_CHECKER_H
#define ADDR_CHECKER_H 1

#include <multiboot2.h>
#include <stdint.h>

uint64_t get_M2IS_max_addr(const M2IS *mis);

#endif
