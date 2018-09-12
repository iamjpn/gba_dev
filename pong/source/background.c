#include "background.h"
#include <gba_console.h>
#include <gba_video.h>
#include <gba_sprites.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>
#include "backgrounddata.h"
#include "sprite.h"

/* background_init loads the tiles and pallete into background memory */
void background_init() {
  memcpy(BG_COLORS, starbackgroundPal, starbackgroundPalLen);
  memcpy(&tile_mem[0][0], starbackgroundTiles, starbackgroundTilesLen);
  memcpy(&se_mem[30][0], starbackgroundMap, starbackgroundMapLen);
  REG_BG0CNT =
      CHAR_BASE(0) | SCREEN_BASE(30) | BG_256_COLOR | BG_SIZE_0 | BG_MOSAIC;
}

/* background_update moves the background */
void background_update() {
  static int ofs;
  REG_BG0HOFS = ofs++;
}