#include <stdbool.h>
#include <kernel/vga.h>
#include <kernel/ioports.h>

void VGA_set_blink(const bool blink) {
  // http://www.osdever.net/FreeVGA/vga/attrreg.htm
  // port 0x3c0, Attribute Mode Control Register (Index 10h)
  // controls whether blink is enabled

  // reset 0x3c0 to index mode
  // From OSDev Wiki:
  // "This port is a confusing one: you write both the index and data bytes to the same port. The VGA keeps track of whether the next write is supposed to be the index or the data byte. However, the initial state is unknown. By reading from port 0x3DA it'll go to the index state."
  inb(0x3DA);

  // write the index for the AMCR
  // http://www.osdever.net/FreeVGA/vga/attrreg.htm
  // bit 5 of this register is the Palette Address Source
  // if this bit is 0, the controller enters a mode for loading
  // colours into the internal palette registers. Only if this bit is
  // set do we get normal access to the AMCR.
  outb(0x3C0, 0x10 | (1<<5));

  // read the current contents of the AMCR
  io_byte AMCR = inb(0x3C1);

  // set/clear the blink bit
  if (blink) {
    AMCR |= 1 << 3;
  } else {
    AMCR &= ~((io_byte)(1 << 3));
  }

  // reset 0x3c0 to index mode again
  inb(0x3DA);
  // write the AMCR index
  outb(0x3C0, 0x10 | (1<<5));
  // write the modified AMCR
  outb(0x3C0, AMCR);
}
