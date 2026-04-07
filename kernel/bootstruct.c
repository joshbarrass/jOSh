#include <stdbool.h>
#include <kernel/bootstruct.h>
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
  // memcpy the colour info
  // TODO: something cleaner
  for (size_t i = 0; i < sizeof(src->color_info); ++i) {
    ((uint8_t*)&dst->color_info)[i] = ((uint8_t*)&src->color_info)[i];
  }
  return true;
}
