#ifndef __PIC_8259_H
#define __PIC_8259_H

#include <stdint.h>

#define PIC_MASTER_IO_PORT_CMD 0x20
#define PIC_MASTER_IO_PORT_DATA 0x21
#define PIC_SLAVE_IO_PORT_CMD 0xA0
#define PIC_SLAVE_IO_PORT_DATA 0XA1

// Initialisation Command Words (ICWs)
// https://helppc.netcore2k.net/hardware/8259
#define ICW1_INIT (1 << 4)
#define ICW1_LEVEL (1 << 3) // level triggered mode -- default is edge triggered mode
#define ICW1_4BYTE (1 << 2) // 4-byte interrupt vectors -- default is 8 byte
#define ICW1_SINGLE (1 << 1) // single 8259 -- default is cascade
#define ICW1_ICW4 (1 << 0) // ICW4 will be present

// ICW2 is the offset for the interrupt vectors
// 3 least significant bits should be 0 on x86, meaning the offset must be a multiple of 8
// Typical choice is 0x20/0d32 master, 0x28/0d40 slave

// ICW3 informs the PICs about the cascade
// Master PIC: (1 << N) set = Slave PIC on IRQ N
// Slave PIC: Simply the IRQ of the Slave PIC
// On x86, the slave PIC is on IRQ 2
#define PIC_SLAVE_IRQ (2)
#define ICW3_MASTER (1 << PIC_SLAVE_IRQ)
#define ICW3_SLAVE PIC_SLAVE_IRQ

#define ICW4_8086 (1 << 0) // 8086 mode
#define ICW4_AUTO_EOI (1 << 1) // auto end-of-interrupt -- default is normal EOI
#define ICW4_BUFFERED_SLAVE (0x8)
#define ICW4_BUFFERED_BOTH (0xC)
#define ICW4_SFNM (1 << 4) // special fully nested mode

void PIC_remap(const uint8_t offset1, const uint8_t offset2);
void PIC_disable();

#endif
