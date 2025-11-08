#ifndef __MEMORY_VMM_H
#define __MEMORY_VMM_H

#include <stddef.h>
#include <stdint.h>

typedef struct VAddressSpace VAddressSpace;

typedef uint64_t flags_t;

void vmm_init();

// vmm_kmap maps a contiguous physical address space into a the kernel's address space.
// Args:
//   - phys_addr: physical address to map into the address space
//   - size: size in bytes to map into the address space. If this is a multiple of the page size, you can generally assume that the memory will be mapped exactly. If this is not a multiple of the page size, it is up to the implementation as to how to handle this.
//   - virt_addr: virtual address to map the physical memory to. If this is 0, the kernel will find an address by itself. If this is non-zero, the kernel will try to map it there; if the destination is already in use, it will search for a free address above the requested address.
//   - flags: flags to adjust the behaviour -- reserved for future use!
void* vmm_kmap(uintptr_t phys_addr, const size_t size, uintptr_t virt_addr, flags_t flags);
// vmm_unmap removes a contiguous virtual address space from the kernel's address space.
// Args:
//   - virt_addr: virtual address to unmap.
//   - size: size in bytes to unmap
void* vmm_kunmap(uintptr_t virt_addr, const size_t size);

// mmap/munmap -- user-space memory
// Args are mostly the same as the kspace equivalents, but accepts an
// address space argument. This is not needed for kspace mappings
// since kernel space is held in the address space of all processes,
// but it is needed to manipulate an individual process's userspace
// memory.
void* vmm_mmap(VAddressSpace *addrspace, uintptr_t phys_addr, const size_t size, uintptr_t virt_addr, flags_t flags);
void* vmm_munmap(VAddressSpace *addrspace, uintptr_t virt_addr, const size_t size);

#endif
