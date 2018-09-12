#ifndef SPRITE_H
#define SPRITE_H

// tile 8x8@4bpp: 32bytes; 8 ints
typedef struct { u32 data[8];  } TILE, TILE4;
// d-tile: double-sized tile (8bpp)
typedef struct { u32 data[16]; } TILE8;
// tile block: 32x16 tiles, 16x16 d-tiles
typedef TILE  CHARBLOCK[512];
typedef TILE8 CHARBLOCK8[256];

void oam_copy(OBJATTR *dst, const OBJATTR *src, u32 count);
void oam_init(OBJATTR *obj, u32 count);

#define tile_mem  ( (CHARBLOCK*)0x06000000)
#define tile8_mem ((CHARBLOCK8*)0x06000000)

typedef u16 SCR_ENTRY, SE;
typedef SCR_ENTRY	SCREENBLOCK[1024];
#define se_mem			((SCREENBLOCK*)0x06000000)

/* Used a double buffer so that only need to access the VRAM once */
#define MAX_SPRITES 128
OBJATTR obj_buffer[MAX_SPRITES];

/* hide_sprite turns off the display attribute for a sprite object */
INLINE void 
hide_sprite(OBJATTR *sprite) 
{
	sprite->attr0 |= (1<<9);
}

/* show_sprite turns on the display attribute for a sprite object */
INLINE void 
show_sprite(OBJATTR *sprite) 
{
	sprite->attr0 &= ~(1<<9);
}


#endif
