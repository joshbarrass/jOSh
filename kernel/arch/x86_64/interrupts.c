#include <stdint.h>
#include <kernel/interrupts.h>
#include <kernel/x86_64/interrupts.h>
#include <kernel/panic.h>

static GateDescriptor64 IDT[256];
static struct {
  uint16_t size;
  uintptr_t offset;
} __attribute__((packed)) IDTD;

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

__attribute__ ((sysv_abi))
void do_df() {
  kpanic("Double fault!");
  return;
}

extern void df_handler();

void setup_interrupts() {
  build_gate_descriptor_64(&IDT[8], (void *)&df_handler, 8, 0, 0xF, 0, true);
  IDTD.offset = (uintptr_t)IDT;
  IDTD.size = sizeof(GateDescriptor64) * 256;
  __asm__ volatile("lidt (%0)\r\n" ::"r"(&IDTD));
}
