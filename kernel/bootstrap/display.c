#include <kernel/bootstrap/display.h>
#include <kernel/bootstrap/static_bump.h>
#include <kernel/drivers/console.h>
#include <kernel/drivers/ega/ega.h>
#include <kernel/drivers/bitmap_console/bitmap_console.h>
#include <kernel/drivers/null_console/null_console.h>
#include <multiboot2.h>

static _Alignas(uint64_t) uint8_t buf[256];
static StaticBumper bumper = STATIC_BUMP_ALLOCATOR(buf);

static const m2is_framebuffer_info *get_fbinfo(const M2IS *m2is) {
  if (m2is == NULL) return NULL;
  m2is_tag_iterator iter = new_m2is_iterator(m2is);
  const m2is_tag *tag = m2is_iterator_next(&iter);
  while (tag != NULL) {
    if (tag->type == M2IS_TYPE_FRAMEBUFFER) return (m2is_framebuffer_info *)tag;
    tag = m2is_iterator_next(&iter);
  }
  return NULL;
}

static ConsoleDriver *bootstrap_ega_driver(const void* addr, const size_t width, const size_t height, const size_t pitch) {
  EGAConsole * const drv = SB_ALLOC_ALIGNED(&bumper, EGAConsole);
  ega_driver_init(drv, addr, width, height, pitch);
  return (ConsoleDriver*)drv;
}

static ConsoleDriver *bootstrap_bitmap_driver(const void* addr, const size_t width, const size_t height, const size_t pitch, const uint8_t bpp, color_info_direct color_info) {
  BitmapConsole * const drv = SB_ALLOC_ALIGNED(&bumper, BitmapConsole);
  bitmap_console_init(drv, addr, width, height, pitch, bpp, color_info);
  return (ConsoleDriver*)drv;
}

ConsoleDriver *bootstrap_console_driver_m2is(const M2IS *m2is) {
  const m2is_framebuffer_info *fbinfo = get_fbinfo(m2is);
  if (fbinfo == NULL) return get_null_console();
  switch (fbinfo->type) {
  case M2IS_FB_TYPE_TEXT:
    return bootstrap_ega_driver((void*)(uintptr_t)fbinfo->addr, fbinfo->width, fbinfo->height, fbinfo->pitch);
  case M2IS_FB_TYPE_DIRECT: {
    const color_info_direct color_info = {
      .red_bits = fbinfo->color_info.direct.red_bits,
      .red_offset = fbinfo->color_info.direct.red_offset,
      .green_bits = fbinfo->color_info.direct.green_bits,
      .green_offset = fbinfo->color_info.direct.green_offset,
      .blue_bits = fbinfo->color_info.direct.blue_bits,
      .blue_offset = fbinfo->color_info.direct.blue_offset};
    return bootstrap_bitmap_driver((void *)(uintptr_t)fbinfo->addr,
                                   fbinfo->width, fbinfo->height, fbinfo->pitch,
                                   fbinfo->bpp, color_info);
  }
  case M2IS_FB_TYPE_INDEXED:
    break;
  }
  return get_null_console();
}

ConsoleDriver *bootstrap_console_driver_bootstruct(const BootStruct *bootstruct) {
  if (!bs_verify_checksum(bootstruct) || (bootstruct->flags & BS_FLAG_FRAMEBUFFER) == 0) return get_null_console();
  const BootStruct_fbinfo *fbinfo = &bootstruct->fbinfo;
  switch (fbinfo->type) {
  case BS_FB_TYPE_TEXT:
    return bootstrap_ega_driver((void*)(uintptr_t)fbinfo->virt_addr, fbinfo->width, fbinfo->height, fbinfo->pitch);
  case BS_FB_TYPE_DIRECT:
    return bootstrap_bitmap_driver((void *)(uintptr_t)fbinfo->virt_addr,
                                     fbinfo->width, fbinfo->height,
                                     fbinfo->pitch, fbinfo->bpp,
                                     fbinfo->color_info.direct);
  case BS_FB_TYPE_INDEXED:
  case BS_FB_TYPE_UNKNOWN:
    break;
  }
  return get_null_console();
}
