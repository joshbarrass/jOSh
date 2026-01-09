#ifndef __X86_64_PAGING_H
#define __X86_64_PAGING_H

#include <stdint.h>

uint64_t read_CR0();
uint64_t read_CR2();
uint64_t read_CR3();
uint64_t read_CR4();

#endif
