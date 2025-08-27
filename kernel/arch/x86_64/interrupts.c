#include <stdint.h>
#include <kernel/interrupts.h>
#include <kernel/x86_64/interrupts.h>
#include <kernel/panic.h>

static GateDescriptor64 IDT[256];
static struct {
  uint16_t size;
  uintptr_t offset;
} __attribute__((packed)) IDTD;

typedef struct __attribute__((packed)) {
  uint64_t r15;
  uint64_t r14;
  uint64_t r13;
  uint64_t r12;
  uint64_t r11;
  uint64_t r10;
  uint64_t r9;
  uint64_t r8;
  uint64_t rbp;
  uint64_t rsp;
  uint64_t rdi;
  uint64_t rsi;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rbx;
  uint64_t rax;

  uint64_t err_code;
  // based on https://wiki.osdev.org/Interrupt_Service_Routines#Clang
  uint64_t ret_ip;
  uint64_t ret_cs;
  uint64_t ret_flags;
  uint64_t ret_sp;
  uint64_t ret_ss;
} InterruptStackFrame;

static void
build_gate_descriptor_64(GateDescriptor64 *gd, void *interrupt,
                         uint16_t segment, char ist, char gate_type, char dpl,
                         bool present) {
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
void do_df(InterruptStackFrame *fr) {
  kpanic("Double fault!\n"
         "Error code: %llx\n"
         "Fault occured executing: %#018llx\n\n"
         "Registers:\n"
         "rax: %#018llx  rbx: %#018llx\n"
         "rcx: %#018llx  rdx: %#018llx\n"
         "rsi: %#018llx  rdi: %#018llx\n"
         "rsp: %#018llx  rbp: %#018llx\n"
         " r8: %#018llx   r9: %#018llx\n"
         "r10: %#018llx  r11: %#018llx\n"
         "r12: %#018llx  r13: %#018llx\n"
         "r14: %#018llx  r15: %#018llx\n"
         " cs: %#018llx   ss: %#018llx\n\n"
         "CPU Flags: %#018llx\n",
         fr->err_code,
         fr->ret_ip, fr->rax, fr->rbx,
         fr->rcx, fr->rdx, fr->rsi,
         fr->rdi, fr->ret_sp, fr->rbp,
         fr->r8, fr->r9, fr->r10,
         fr->r11, fr->r12, fr->r13,
         fr->r14, fr->r15, fr->ret_cs,
         fr->ret_ss, fr->ret_flags);
  return;
}

__attribute__ ((sysv_abi))
void do_pf(InterruptStackFrame *fr) {
  kpanic("Page Fault!\n"
         "Error code: %llx\n"
         "Fault occured executing: %#018llx\n\n"
         "Registers:\n"
         "rax: %#018llx  rbx: %#018llx\n"
         "rcx: %#018llx  rdx: %#018llx\n"
         "rsi: %#018llx  rdi: %#018llx\n"
         "rsp: %#018llx  rbp: %#018llx\n"
         " r8: %#018llx   r9: %#018llx\n"
         "r10: %#018llx  r11: %#018llx\n"
         "r12: %#018llx  r13: %#018llx\n"
         "r14: %#018llx  r15: %#018llx\n"
         " cs: %#018llx   ss: %#018llx\n\n"
         "CPU Flags: %#018llx\n",
         fr->err_code,
         fr->ret_ip, fr->rax, fr->rbx,
         fr->rcx, fr->rdx, fr->rsi,
         fr->rdi, fr->ret_sp, fr->rbp,
         fr->r8, fr->r9, fr->r10,
         fr->r11, fr->r12, fr->r13,
         fr->r14, fr->r15, fr->ret_cs,
         fr->ret_ss, fr->ret_flags);
  return;
}

extern void df_handler();
extern void pf_handler();

void setup_interrupts() {
  build_gate_descriptor_64(&IDT[8], (void *)&df_handler, 8, 0, 0xF, 0, true);
  build_gate_descriptor_64(&IDT[14], (void *)&pf_handler, 8, 0, 0xF, 0, true);
  IDTD.offset = (uintptr_t)IDT;
  IDTD.size = sizeof(GateDescriptor64) * 256;
  __asm__ volatile("lidt (%0)\r\n" ::"r"(&IDTD));
}
