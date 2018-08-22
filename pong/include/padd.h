
//{{BLOCK(padd)

//======================================================================
//
//	padd, 8x32@8, 
//	+ palette 256 entries, not compressed
//	+ 4 tiles Metatiled by 1x4 not compressed
//	Total size: 512 + 256 = 768
//
//	Time-stamp: 2017-07-26, 23:34:54
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.6
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_PADD_H
#define GRIT_PADD_H

#define paddTilesLen 256
extern const unsigned int paddTiles[64];

#define paddaTilesLen 64
extern const unsigned int paddaTiles[16];


#define paddPalLen 512
extern const unsigned int paddPal[128];

#endif // GRIT_PADD_H

//}}BLOCK(padd)
