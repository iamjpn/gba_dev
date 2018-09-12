#include <gba_console.h>
#include <gba_sprites.h>
#include "sprite.h"

void oam_init(OBJATTR *obj, u32 count) {
  u32 nn = count;
  u32 *dst = (u32 *)obj;

  // Hide each object
  while (nn--) {
    *dst++ = OBJ_DISABLE;
    *dst++ = 0;
  }
  // init oam
  oam_copy(OAM, obj, count);
}

void oam_copy(OBJATTR *dst, const OBJATTR *src, u32 count) {
  while (count--)
    *dst++ = *src++;
}
