#ifndef ADDR_CHECKER_H
#define ADDR_CHECKER_H 1

#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"
#include "multiboot.h"

bool check_all(const uint64_t load_addr, const MIS *mis);

#endif
