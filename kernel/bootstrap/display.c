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

static ConsoleDriver *bootstrap_ega_driver(const m2is_framebuffer_info *fbinfo) {
  EGAConsole * const drv = SB_ALLOC_ALIGNED(&bumper, EGAConsole);
  ega_driver_init(drv, (void*)(uintptr_t)fbinfo->addr, fbinfo->width, fbinfo->height, fbinfo->pitch);
  return (ConsoleDriver*)drv;
}

static ConsoleDriver *bootstrap_bitmap_driver(const m2is_framebuffer_info *fbinfo) {
  BitmapConsole * const drv = SB_ALLOC_ALIGNED(&bumper, BitmapConsole);
  bitmap_console_init(drv, fbinfo);
  return (ConsoleDriver*)drv;
}

ConsoleDriver *bootstrap_console_driver(const M2IS *m2is) {
  const m2is_framebuffer_info *fbinfo = get_fbinfo(m2is);
  if (fbinfo == NULL) return get_null_console();
  switch (fbinfo->type) {
  case 2:
    return bootstrap_ega_driver(fbinfo);
  case 1:
    return bootstrap_bitmap_driver(fbinfo);
  }
  return get_null_console();
}
