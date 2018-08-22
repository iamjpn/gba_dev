
//{{BLOCK(starbackground)

//======================================================================
//
//	starbackground, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 3 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 32x32 
//	Total size: 512 + 192 + 2048 = 2752
//
//	Time-stamp: 2018-07-06, 19:17:23
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STARBACKGROUND_H
#define GRIT_STARBACKGROUND_H

#define starbackgroundTilesLen 192
extern const unsigned short starbackgroundTiles[96];

#define starbackgroundMapLen 2048
extern const unsigned short starbackgroundMap[1024];

#define starbackgroundPalLen 512
extern const unsigned short starbackgroundPal[256];

#endif // GRIT_STARBACKGROUND_H

//}}BLOCK(starbackground)
