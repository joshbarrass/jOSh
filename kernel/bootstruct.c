#include <stdbool.h>
#include <kernel/bootstruct/bootstruct.h>
#include <multiboot2.h>

void bs_init(BootStruct *bs) {
  bs->flags = 0;
  bs->checksum = 0;
  bs->M2IS = 0;
  bs->lowest_free_addr = 0;
}

bool bs_verify_checksum(const BootStruct *bs) {
  return (bs_flags_t)(BOOTSTRUCT_MAGIC + bs->flags + bs->checksum) == 0;
}

static inline bs_flags_t bs_gen_checksum(const BootStruct *bs) {
  return (bs_flags_t)(-(BOOTSTRUCT_MAGIC + bs->flags));
}

void bs_set_checksum(BootStruct *bs) {
  bs->checksum = bs_gen_checksum(bs);
}

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

bool bs_convert_fbinfo(BootStruct *bs, const M2IS *m2is) {
  const m2is_framebuffer_info *src = get_fbinfo(m2is);
  if (src == NULL) return false;
  BootStruct_fbinfo * const dst = &bs->fbinfo;
  dst->phys_addr = src->addr;
  dst->virt_addr = src->addr;
  dst->type = m2is_fb_type_to_bs(src->type);
  dst->width = src->width;
  dst->height = src->height;
  dst->pitch = src->pitch;
  dst->bpp = src->bpp;
  // convert the colour info based on the type
  switch (dst->type) {
  case BS_FB_TYPE_TEXT:
  case BS_FB_TYPE_UNKNOWN:
    break;
  case BS_FB_TYPE_DIRECT:
    dst->color_info.direct.red_bits = src->color_info.direct.red_bits;
    dst->color_info.direct.red_offset = src->color_info.direct.red_offset;
    dst->color_info.direct.green_bits = src->color_info.direct.green_bits;
    dst->color_info.direct.green_offset = src->color_info.direct.green_offset;
    dst->color_info.direct.blue_bits = src->color_info.direct.blue_bits;
    dst->color_info.direct.blue_offset = src->color_info.direct.blue_offset;
    break;
  case BS_FB_TYPE_INDEXED:
    dst->color_info.indexed.num_colors = src->color_info.indexed.num_colors;
    // not super safe, but fine so long as the multiboot struct is
    // mapped.
    dst->color_info.indexed.palette = (bs_ptr_t)(uintptr_t)&src->color_info.indexed.palette;
  }
  return true;
}
