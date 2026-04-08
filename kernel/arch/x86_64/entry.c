#include <multiboot2.h>
#include <kernel/bootstruct/bootstruct.h>

extern const BootStruct *bootstruct;
extern const M2IS *mis;
extern void kernel_main();

__attribute__((noreturn,sysv_abi)) void _entry_c(uint32_t rax, uint32_t *rbx) {
  // initialise the loader pointers to ensure they're zero to start
  // with
  mis = NULL;
  bootstruct = NULL;

  // if we were booted directly by a multiboot2-compliant bootloader,
  // we can save the pointer to the M2IS
  if ((uint32_t)rax == (uint32_t)0x36d76289) {
    mis = (M2IS*)rbx;
  }
  // if we were booted by a custom jOSh loader, we can load the custom
  // bootstruct and extract the MIS from it (if present)
  else if (rax == (uint32_t)BOOTSTRUCT_MAGIC) {
    if (bs_verify_checksum((BootStruct *)rbx)) {
      bootstruct = (BootStruct *)rbx;
      if (bootstruct->flags & BS_FLAG_MIS) mis = bs_get_MIS(bootstruct);
    }
  }
  kernel_main();

  // create a footer to hang if the kernel ever returns here
  while (1) {
    asm volatile(".hang:\r\n"
                 "cli\r\n"
                 "hlt\r\n");
  }
}
