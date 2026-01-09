#ifndef __MEMORY_VMM_H
#define __MEMORY_VMM_H

#include <stddef.h>
#include <stdint.h>
#include <kernel/memory/types.h>

typedef struct VAddressSpace VAddressSpace;

typedef uint64_t flags_t;

void vmm_init();

// vmm_kmap maps a contiguous physical address space into a the kernel's address space.
// Args:
//   - phys_addr: physical address to map into the address space. Does not necessarily have to be page-aligned.
//   - size: size in bytes to map into the address space. If this is a multiple of the page size, you can generally assume that the memory will be mapped exactly. If this is not a multiple of the page size, it is up to the implementation as to how to handle this.
//   - virt_addr: virtual address to map the physical memory to. If this is 0, the kernel will find an address by itself. If this is greater than or equal to the start of kernel-space, the kernel will try to map it exactly where you request; if the requested address lies outside kernel-space or the destination is already in use, it will search for a free address in kernel-space above the requested address.
//   - flags: flags to adjust the behaviour -- reserved for future use!
virt_addr_t vmm_kmap(phys_addr_t phys_addr, const size_t size, virt_addr_t virt_addr, flags_t flags);
// vmm_unmap removes a contiguous virtual address space from the kernel's address space.
// Args:
//   - virt_addr: virtual address to unmap.
//   - size: size in bytes to unmap
void vmm_kunmap(virt_addr_t virt_addr, const size_t size);

// mmap/munmap -- user-space memory
// Args are mostly the same as the kspace equivalents, but accepts an
// address space argument. This is not needed for kspace mappings
// since kernel space is held in the address space of all processes,
// but it is needed to manipulate an individual process's userspace
// memory.
virt_addr_t vmm_mmap(VAddressSpace *addrspace, phys_addr_t phys_addr, const size_t size, virt_addr_t virt_addr, flags_t flags);
void vmm_munmap(VAddressSpace *addrspace, virt_addr_t virt_addr, const size_t size);

// translate a virtual address in the current CR3 to a physical
// address. Returns -1 if the address is not mapped (which should be
// an invalid on basically any real computer).
phys_addr_t vmm_get_phys(const virt_addr_t virt_addr);

#endif
