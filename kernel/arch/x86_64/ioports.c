#include <kernel/ioports.h>

io_byte inb(const io_port_num port) {
  io_byte val;
  asm volatile (
                "inb %1, %0\r\n"
                : "=a" (val)
                : "id" (port)
                :
                );
  return val;
}

void outb(const io_port_num port, const io_byte val) {
  asm volatile (
                "outb %1, %0\r\n"
                :
                : "id" (port), "a" (val)
                :
                );
}

io_word inw(const io_port_num port) {
  io_word val;
  asm volatile (
                "inw %1, %0\r\n"
                : "=a" (val)
                : "id" (port)
                :
                );
  return val;
}

void outw(const io_port_num port, const io_word val) {
  asm volatile (
                "outw %1, %0\r\n"
                :
                : "id" (port), "a" (val)
                :
                );
}

io_dword inl(const io_port_num port) {
  io_dword val;
  asm volatile (
                "inl %1, %0\r\n"
                : "=a" (val)
                : "id" (port)
                :
                );
  return val;
}

void outl(const io_port_num port, const io_dword val) {
  asm volatile (
                "outl %1, %0\r\n"
                :
                : "id" (port), "a" (val)
                :
                );
}
