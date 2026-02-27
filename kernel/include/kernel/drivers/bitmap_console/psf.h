#ifndef __DRIVERS_PSF_FONT_H
#define __DRIVERS_PSF_FONT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PSF1_FONT_MAGIC 0x0436
// https://en.wikipedia.org/wiki/PC_Screen_Font#Font_modes
#define PSF1_MODE512 1
#define PSF1_MODEHASTAB 2
#define PSF1_MODESEQ 4

// Based on https://wiki.osdev.org/PC_Screen_Font
/*
    For PSF1 glyph width is always = 8 bits
    and glyph height = characterSize
*/
typedef struct {
  uint16_t magic; // Magic bytes for identification.
  uint8_t fontMode; // PSF font mode.
  uint8_t characterSize; // PSF character size.
  uint8_t data[];
} PSF1;

/* ---------- */

bool is_PSF1_font(const void *);
const uint8_t *PSF1_get_glyph(const PSF1*, size_t);

#endif
