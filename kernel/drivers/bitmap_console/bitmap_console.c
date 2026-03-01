#include <kernel/drivers/bitmap_console/bitmap_console.h>
#include <kernel/drivers/bitmap_console/psf.h>
#include <kernel/drivers/null_console/null_console.h>

extern const char _binary_default_font_psf_start;
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

inline static uint32_t *get_row_32bpp(BitmapConsole *drv, const size_t y) {
  return (uint32_t*)(drv->addr + y*drv->pitch);
}

static void putch_32bpp(ConsoleDriver * console, const ScreenChar c, const size_t x, const size_t y) {
  BitmapConsole *drv = (BitmapConsole*)console;
  const uint8_t *glyph = &default_font->data[c.character * default_font->characterSize];
  for (size_t i = 0; i < default_font->characterSize; ++i) {
    const uint8_t glyph_row = glyph[i];
    for (size_t j = 0; j < 8; ++j) {
      const uint8_t glyph_pix = glyph_row & (128 >> j);
      const uint32_t color = (glyph_pix == 0) ? c.color.bg : c.color.fg;
      uint32_t *row = get_row_32bpp(drv, y*default_font->characterSize + i);
      row[x*8 + j] = drv->palette_32bpp[color];
    }
  }
}

static void line_feed_32bpp(ConsoleDriver *console, const CharColor c) {}

static void clear_32bpp(ConsoleDriver *console, const CharColor color) {
  BitmapConsole *drv = (BitmapConsole*)console;
  for (size_t y = 0; y < drv->height_px; ++y) {
    for (size_t x = 0; x < drv->width_px; ++x) {
      uint32_t * row = get_row_32bpp(drv, y);
      row[x] = drv->palette_32bpp[color.bg];
    }
  }
}

void bitmap_console_init(BitmapConsole *drv, m2is_framebuffer_info *fbinfo) {
  drv->addr = (uint8_t*)(uintptr_t)fbinfo->addr;
  drv->pitch = fbinfo->pitch;
  drv->width_px = fbinfo->width;
  drv->drv.width = fbinfo->width / 8; // PSF1 has fixed width of 8 pixels
  drv->height_px = fbinfo->height;
  drv->drv.height = fbinfo->height / default_font->characterSize;
  drv->bpp = fbinfo->bpp;
  drv->color_info = fbinfo->color_info.direct;

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
  } else { // unhandled bpp -- TODO: use unoptimised generic routines
    // (for now, just use the null driver)
    drv->drv = *get_null_console();
  }
}
