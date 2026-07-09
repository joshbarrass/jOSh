#include <kernel/memory/dynamic.h>
#include <kernel/memory/constants.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
#include <kernel/panic.h>

static void _kfree_fullpage(const malloc_header_t *header) {
  // copy everything to local variables before unmapping -- we don't
  // want the compiler to access the header again after it's been
  // unmapped.
  const phys_addr_t phys_page = vmm_get_phys((virt_addr_t)header);
  const size_t malloc_size = header->size;
  // TODO: should we zero the page before unmapping it?
  vmm_kunmap((virt_addr_t)header, malloc_size);
  pmm_free_pages(phys_page, malloc_size / PAGE_SIZE);
}

void kfree(void *const addr) {
  // locate the header
  const malloc_header_t *header = (const malloc_header_t*)(((const uint8_t*)addr) - sizeof(malloc_header_t));

  // dispatch the appropriate kfree based on the type in the header
  switch (header->type) {
  case MALLOC_TYPE_FULLPAGE:
    _kfree_fullpage(header);
    break;
  default:
    kpanic("Unknown malloc type in kmalloc header: %#x", header->type);
    break;
  }
  return;
}
