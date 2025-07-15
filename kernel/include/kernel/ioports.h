#ifndef __IOPORTS_H
#define __IOPORTS_H

#include <stdint.h>

typedef uint16_t io_port_num;
typedef uint8_t io_byte;
typedef uint16_t io_word;
typedef uint32_t io_dword;

io_byte inb(const io_port_num port);
void outb(const io_port_num port, const io_byte val);
io_word inw(const io_port_num port);
void outw(const io_port_num port, const io_word val);
io_dword inl(const io_port_num port);
void outl(const io_port_num port, const io_dword val);

#endif
