#include <kernel/bootstruct.h>

void bs_init(BootStruct *bs) {
  bs->flags = 0;
  bs->checksum = 0;
  bs->MIS = 0;
}

bool bs_verify_checksum(const BootStruct *bs) {
  return (bs_flags_t)(BOOTSTRUCT_MAGIC + bs->flags + bs->checksum) == 0;
}

static inline bs_flags_t bs_gen_checksum(const BootStruct *bs) {
  return (bs_flags_t)(-(BOOTSTRUCT_MAGIC + bs->flags));
}

void bs_set_checksum(BootStruct *bs) {
  bs->checksum = bs_gen_checksum(bs);
}
