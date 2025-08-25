#include <stdint.h>
#include <kernel/interrupts.h>
#include <kernel/x86_64/interrupts.h>

static void build_gate_descriptor_64(GateDescriptor64 *gd, void *interrupt,
                                     uint16_t segment, char ist, char gate_type,
                                     char dpl, bool present) {
  uint64_t offset = (uint64_t)(uintptr_t)interrupt;
  gd->offset_0 = offset & 0xffffffff;
  offset >>= 16;
  gd->offset_1 = offset & 0xffffffff;
  offset >>= 16;
  gd->offset_2 = offset & 0xffffffffffffffff;

  gd->segment = segment;
  gd->ist = ist;
  gd->gate_type = gate_type;
  gd->dpl = dpl;
  gd->present = present;

  gd->__reserved_0 = 0;
  gd->__reserved_1 = 0;
  gd->__reserved_2 = 0;
}
