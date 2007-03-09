#ifndef SURFACE_H_INCLUDED
#define SURFACE_H_INCLUDED

#include "dmc.h"


typedef struct {
  Uint16 w, h;
  Uint8 bytesPerPixel;
  Uint16 pitch;
  Uint8 *pixels;
} Surface;

typedef struct {
  Sint16 x, y;
  Uint16 w, h;
} Rect;


/* Prototypes */
Surface *newSurf(Uint16 w, Uint16 h, Uint8 bytesPerPixel, Uint16 pitch);
Surface *newSurfFromC4(C4Img *img);
Surface *newSurfFromC8(C8Img *img);
void freeSurf(Surface *surf);

Rect *newRect(Sint16 x, Sint16 y, Uint16 w, Uint16 h);
Surface *flipSurface(Surface *img);
void bltSurface(Surface *src, Rect *srcr, Surface *dst, Rect *dstr);

#endif /* SURFACE_H_INCLUDED */
