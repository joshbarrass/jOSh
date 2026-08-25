#include "kernel/x86_64/pic/pic8259.h"
#include "kernel/x86_64/ioports.h"

// Writes to an unused IO port to create a small artificial delay.
// Useful to give the PIC some time to respond to commands before
// hammering it with the next command.
// https://wiki.osdev.org/Inline_Assembly/Examples#IO_WAIT
static inline void io_wait() {
  outb(0x80, 0);
}

static inline void pic1_cmd(const io_byte cmd) {
  outb(PIC_MASTER_IO_PORT_CMD, cmd);
  io_wait();
}

static inline void pic2_cmd(const io_byte cmd) {
  outb(PIC_SLAVE_IO_PORT_CMD, cmd);
  io_wait();
}

static inline void pic1_data(const io_byte data) {
  outb(PIC_MASTER_IO_PORT_DATA, data);
  io_wait();
}

static inline void pic2_data(const io_byte data) {
  outb(PIC_SLAVE_IO_PORT_DATA, data);
  io_wait();
}

void PIC_remap(const uint8_t offset1, const uint8_t offset2) {
  // send the initialisation command
  // on x86, we need to use ICW4 to tell it to operate in 8086 mode
  // init command is initially on the command port
  // subsequent ICWs are sent on the data port
  pic1_cmd(ICW1_INIT | ICW1_ICW4);
  pic2_cmd(ICW1_INIT | ICW1_ICW4);
  // PICs now waiting for ICWs 2-4
  // ICW2 is the offset for the interrupt vectors
  pic1_data(offset1);
  pic2_data(offset2);
  // ICW3 is the cascade setup
  pic1_data(ICW3_MASTER);
  pic2_data(ICW3_SLAVE);
  // ICW4 is extra info -- at this time, we just tell it to use 8086
  // mode
  pic1_data(ICW4_8086);
  pic2_data(ICW4_8086);
  // init command is now complete
  // unmask both PICs before we return
  // writing to the data port when there's no active command sets the
  // mask for that PIC -- writing 0 clears the mask
  pic1_data(0);
  pic2_data(0);
}

void PIC_disable() {
  pic1_data(0xff);
  pic2_data(0xff);
}
