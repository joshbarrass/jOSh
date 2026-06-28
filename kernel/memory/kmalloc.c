#include <kernel/memory/dynamic.h>
#include <kernel/memory/constants.h>
#include <kernel/memory/types.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>

// kmalloc implementation intended for larger allocations that are
// unsuitable to allocate on the heap. This implementation fetches
// enough free pages from the memory manager, maps them into memory,
// and returns the address.
static void *kmalloc_on_pages(const size_t size, const malloc_flags_t flags) {
  const size_t malloc_size = size + sizeof(malloc_header_t);
  const size_t pages_required = (malloc_size + (PAGE_SIZE - 1)) / PAGE_SIZE;

  // we don't need to worry about alignment in this implementation
  // because the header already uses the maximum alignment and the
  // pages are otherwise fully aligned.

  // get the required number of pages from the PMM
  const phys_addr_t pages = pmm_alloc_pages(pages_required);

  // use the VMM to map the pages into kernel memory
  const virt_addr_t allocated = vmm_kmap(pages, malloc_size, NULL, 0);

  // TODO: create the header
  malloc_header_t *header = allocated;

  // we must return the offset to the data, not the malloc header, so
  // the receiver doesn't overwrite the malloc header
  return ((void*)allocated)+sizeof(malloc_header_t);
}

void *kmalloc(const size_t size, const malloc_flags_t flags) {
  // TODO: currently, all allocations use full pages. This is fine for
  // now and a decent way to get started, but eventually we should
  // have the kernel manage its own heap.
  return kmalloc_on_pages(size, flags);
}
