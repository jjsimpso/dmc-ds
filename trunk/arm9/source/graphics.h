#ifndef GRAPHICS_H_INCLUDED

typedef struct {
  Uint16 numItems;
  Uint32 size1;
  Uint16 *size;
  FILE *gfile;
} GfxDat;

typedef struct {
  Uint16 w, h;
  Uint8 pal[6];
  Uint8 *pixels;
} C4Img;

typedef struct {
  Uint16 w, h;
  Uint8 xoff, yoff;
  Uint8 ctype;
  Uint8 *pixels;
} C8Img;

/*
Uint8 FourBitPalMap[16] = {
  234,
  13,
  235,
  12,
  73,
  237,
  11,
  233,
  10,
  71,
  9,
  8,
  231,
  7,
  6,
  97
};
*/

Uint8 DM2FourBitPalMap[16];
extern Uint8 DefaultPalette[256][3];  // initialized in defpal.h

/* Prototypes */
GfxDat *readGfxDat(char *filename);
int *readGfxNdx(char *filename);
void readFourBitPal(FILE *gfxdat, FILE *gfxndx, Uint8 *palmap);
C4Img *loadC4Img(FILE *gfxdat, FILE *gfxndx, int file_num);
C8Img *loadC8Img(FILE *gfxdat, FILE *gfxndx, int file_num);

#endif /* GRAPHICS_H_INCLUDED */
