#include <kernel/psf.h>

bool is_PSF1_font(const void *buf) {
  const PSF1 *psf = (const PSF1*)buf;
  return psf->magic == PSF1_FONT_MAGIC;
}
