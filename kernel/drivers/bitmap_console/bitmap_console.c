#include <kernel/drivers/bitmap_console/bitmap_console.h>
#include <kernel/drivers/bitmap_console/psf.h>

extern char _binary_default_font_psf_start;
static PSF1* default_font = (PSF1*)(&_binary_default_font_psf_start);

static void putch(ConsoleDriver *drv, const ScreenChar c, const size_t x,
                  const size_t y) {

}

static void line_feed(ConsoleDriver *drv, const CharColor c) {}

static void clear(ConsoleDriver *drv, const CharColor color) {}

void bitmap_console_init(BitmapConsole *drv, m2is_framebuffer_info *fbinfo) {
  drv->drv.put_char_at = &putch;
  drv->drv.line_feed = &line_feed;
  drv->drv.clear = &clear;
  drv->addr = (uint8_t*)(uintptr_t)fbinfo->addr;
  drv->width_px = fbinfo->width;
  drv->drv.width = fbinfo->width / 8; // PSF1 has fixed width of 8 pixels
  drv->height_px = fbinfo->height;
  drv->drv.height = fbinfo->height / default_font->characterSize;
  drv->color_info = *(m2is_color_info_direct*)fbinfo->color_info;
}
