#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

/* Size of the graphics.ndx file for DM2 graphics.dat */
#define DM2_GFXNDX_SIZE 22500

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

/* Prototypes */
GfxDat *readGfxDat(char *filename);
int *readGfxNdx(char *filename);
void readFourBitPal(GfxDat *gfxdat, int *gfxndx, Uint8 *palmap);
C4Img *loadC4Img(GfxDat *gfxdat, int *gfxndx, int file_num);
C8Img *loadC8Img(GfxDat *gfxdat, int *gfxndx, int file_num, Uint8 alphaColor);
void freeC4Img(C4Img *img);
void freeC8Img(C8Img *img);
void copyPal24(Uint8 pal[][3], Uint16 *dest, int start, int num);

#endif /* GRAPHICS_H_INCLUDED */
