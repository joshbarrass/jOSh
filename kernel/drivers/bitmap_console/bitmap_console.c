#include <string.h>
#include <kernel/psf.h>
#include <kernel/drivers/bitmap_console/bitmap_console.h>
#include <kernel/drivers/null_console/null_console.h>

extern const char _binary_default_font_psf_start[];
static const PSF1* default_font = (const PSF1*)(&_binary_default_font_psf_start);

// RGB representation of the VGA palette
// Split into 16 triplets, in RGB order
static uint8_t VGA_palette_RGB[16 * 3] =
    {
      0, 0, 0,
      0, 0, 168,
      0, 168, 0,
      0, 168, 168,
      168, 0, 0,
      168, 0, 168,
      168, 87, 0,
      168, 168, 168,
      87, 87, 87,
      87, 87, 255,
      87, 255, 87,
      87, 255, 255,
      255, 87, 87,
      255, 87, 255,
      255, 255, 87,
      255, 255, 255
    };

#define _putch_body(SET_PIXEL_FN)                                       \
  BitmapConsole *drv = (BitmapConsole*)console;                         \
  const uint8_t *glyph = PSF1_get_glyph(default_font, c.character);     \
  for (size_t i = 0; i < default_font->characterSize; ++i) {            \
    const uint8_t glyph_row = glyph[i];                                 \
    for (size_t j = 0; j < 8; ++j) {                                    \
      const uint8_t glyph_pix = glyph_row & (128 >> j);                 \
      const uint32_t color = (glyph_pix == 0) ? c.color.bg : c.color.fg; \
      SET_PIXEL_FN(drv, color, x*8 + j, y*default_font->characterSize + i); \
    }                                                                   \
  }

#define _line_feed_body(SET_PIXEL_FN)                                   \
  BitmapConsole *drv = (BitmapConsole*)console;                         \
  for (size_t y = default_font->characterSize; y < drv->height_px; ++y) { \
    void * const row = (drv->addr + y*drv->pitch);                      \
    void * const shifted_row = (drv->addr + (y-default_font->characterSize)*drv->pitch); \
    memmove(shifted_row, row, sizeof(uint32_t)*drv->width_px);          \
  }                                                                     \
  for (size_t y = (drv->drv.height - 1) * default_font->characterSize;  \
  y < drv->height_px; ++y) {                                            \
    for (size_t x = 0; x < drv->width_px; ++x) {                        \
      SET_PIXEL_FN(drv, c.bg, x, y);                                 \
    }                                                                   \
  }

#define _clear_body(SET_PIXEL_FN)                                       \
  BitmapConsole *drv = (BitmapConsole*)console;                         \
  for (size_t y = 0; y < drv->height_px; ++y) {                         \
    for (size_t x = 0; x < drv->width_px; ++x) {                        \
      SET_PIXEL_FN(drv, color.bg, x, y);                             \
    }                                                                   \
  }

inline static uint32_t *get_row_32bpp(BitmapConsole *drv, const size_t y) {
  return (uint32_t*)(drv->addr + y*drv->pitch);
}

inline static __attribute__((always_inline)) void set_pixel_32bpp(BitmapConsole *drv, const size_t color,
                            const size_t x, const size_t y) {
  uint32_t * const row = get_row_32bpp(drv, y);
  row[x] = drv->palette_32bpp[color];
}

static void putch_32bpp(ConsoleDriver * console, const ScreenChar c, const size_t x, const size_t y) {
  _putch_body(set_pixel_32bpp);
}

static void line_feed_32bpp(ConsoleDriver *console, const CharColor c) {
  _line_feed_body(set_pixel_32bpp);
}

static void clear_32bpp(ConsoleDriver *console, const CharColor color) {
  _clear_body(set_pixel_32bpp);
}

inline static uint8_t *get_row_generic(BitmapConsole *drv, const size_t y) {
  return (uint8_t*)(drv->addr + y*drv->pitch);
}

// TODO: I think all VGA direct colour modes use byte alignment as a
// bare minimum. Instead of working one bit at a time, maybe this can
// be optimised to work one byte at a time.
static inline void set_pixel_channel_generic(BitmapConsole *drv,
                                             const size_t RGB_chan_val,
                                             const size_t x,
                                             const size_t y,
                                             const size_t chan_offset,
                                             const size_t chan_size) {
  uint8_t * const row = get_row_generic(drv, y);
  const size_t chan_val = (RGB_chan_val * ((1 << chan_size) - 1)) / 255;
  for (size_t bit = 0; bit < chan_size; ++bit) {
    const size_t target_bit = x * drv->phys_bpp + chan_offset + bit;
    const size_t target_byte = target_bit / 8;
    const size_t target_offset = target_bit % 8;

    // clear the bit
    row[target_byte] &= ~(1 << target_offset);

    // Find the value (1 or 0) of the appropriate bit in the channel
    // value. We right shift to crop off the lower <bit> bits, then &
    // 1 to get the LSB
    const size_t mask = (chan_val >> bit) & 1;

    // set the bit
    row[target_byte] |= (mask << target_offset);
  }
}

// Method for setting a single pixel's colour bit-by-bit. Slower than
// bpp-optimised routines, but works for any value of bpp.
static inline void set_pixel_generic(BitmapConsole *drv, const size_t color,
                                     const size_t x, const size_t y) {
  set_pixel_channel_generic(drv, VGA_palette_RGB[3*color+0], x, y, drv->color_info.red_offset, drv->color_info.red_bits);
  set_pixel_channel_generic(drv, VGA_palette_RGB[3*color+1], x, y, drv->color_info.green_offset, drv->color_info.green_bits);
  set_pixel_channel_generic(drv, VGA_palette_RGB[3*color+2], x, y, drv->color_info.blue_offset, drv->color_info.blue_bits);
}

static void putch_generic(ConsoleDriver * console, const ScreenChar c, const size_t x, const size_t y) {
  _putch_body(set_pixel_generic);
}

static void line_feed_generic(ConsoleDriver *console, const CharColor c) {
  _line_feed_body(set_pixel_generic);
}

static void clear_generic(ConsoleDriver *console, const CharColor color) {
  _clear_body(set_pixel_generic);
}

void bitmap_console_init(BitmapConsole *drv, const void* addr, const size_t width, const size_t height, const size_t pitch, const uint8_t bpp, const color_info_direct color_info) {
  drv->addr = (uint8_t*)addr;
  drv->pitch = pitch;
  drv->width_px = width;
  drv->drv.width = width / 8; // PSF1 has fixed width of 8 pixels
  drv->height_px = height;
  drv->drv.height = height / default_font->characterSize;
  drv->bpp = bpp;
  // The number of used bits per pixel doesn't need to be a multiple
  // of 8 (e.g. 15bpp modes), but (other than 4bpp palette modes that
  // store two pixels per byte) the hardware prefers the start of each
  // pixel to be byte-aligned because reading/writing a when not
  // byte-aligned becomes awkward very quickly. phys_bpp gives us
  // the stride to the next pixel instead of the number of pixels
  // actually used for image data.
  drv->phys_bpp = (drv->bpp + 7) & ~7;
  drv->color_info = color_info;

  // if we're 32bpp, pre-generate the palette and use the functions
  // optimised for 32bpp
  if (drv->bpp == 32) {
    drv->drv.put_char_at = &putch_32bpp;
    drv->drv.line_feed = &line_feed_32bpp;
    drv->drv.clear = &clear_32bpp;
    for (size_t i = 0; i < 16; ++i) {
      const uint32_t red_max = ((1 << drv->color_info.red_bits) - 1);
      const uint32_t red = red_max*VGA_palette_RGB[3*i]/255;
      const uint32_t green_max = ((1 << drv->color_info.green_bits) - 1);
      const uint32_t green = green_max*VGA_palette_RGB[3*i+1]/255;
      const uint32_t blue_max = ((1 << drv->color_info.blue_bits) - 1);
      const uint32_t blue = blue_max*VGA_palette_RGB[3*i+2]/255;
      drv->palette_32bpp[i] =
        (red << drv->color_info.red_offset) |
        (green << drv->color_info.green_offset) |
        (blue << drv->color_info.blue_offset);
    }
  } else { // unhandled bpp -- use generic routines
    drv->drv.put_char_at = &putch_generic;
    drv->drv.line_feed = &line_feed_generic;
    drv->drv.clear = &clear_generic;
  }
}
