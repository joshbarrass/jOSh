#ifndef ADDR_CHECKER_H
#define ADDR_CHECKER_H 1

#include "multiboot.h"
#include <multiboot2.h>
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"

uint64_t get_MIS_max_addr(const MIS *mis);
uint64_t get_M2IS_max_addr(const M2IS *mis);
bool check_all(const uint64_t load_addr, const MIS *mis);

#endif
