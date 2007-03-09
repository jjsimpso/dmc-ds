#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dmc.h"

#include "surface.h"

/*
  Create a new surface
*/
Surface *newSurf(Uint16 w, Uint16 h, Uint8 bytesPerPixel, Uint16 pitch){
  Surface *s;

  s = (Surface *)malloc(sizeof(Surface));
  if(s == NULL)
    return NULL;

  s->w = w;
  s->h = h;
  s->bytesPerPixel = bytesPerPixel;

  if(pitch == 0)
    s->pitch = w * bytesPerPixel;
  else
    s->pitch = pitch;

  s->pixels = (Uint8 *)malloc(h * s->pitch);

  if(s->pixels == NULL){
    free(s);
    return NULL;
  }

  return s;
}

Surface *newSurfFromC4(C4Img *img){
  Surface *s;

  s = newSurf(img->w, img->h, 1, 0);
  if(s == NULL)
    return NULL;

  memcpy(s->pixels, img->pixels, img->w * img->h);

  return s;
}

Surface *newSurfFromC8(C8Img *img){
  Surface *s;

  s = newSurf(img->w, img->h, 1, 0);
  if(s == NULL)
    return NULL;

  memcpy(s->pixels, img->pixels, img->w * img->h);

  return s;
}

void freeSurf(Surface *surf){
  if(surf->pixels != NULL)
    free(surf->pixels);

  if(surf != NULL)
    free(surf);
}

Surface *flipSurface(Surface *img){
  int i, j, rem, h, pitch;
  Surface *mirror;
  Uint8 *src8, *dest8;
  //Uint32 *src32, *dest32;
  int pixelsPerLine, bytesPerPixel;

  mirror = newSurf(img->w, img->h, img->bytesPerPixel, 0);
  
  bytesPerPixel = img->bytesPerPixel;
  pixelsPerLine = img->w;
  h = img->h;
  pitch = img->pitch;
  
  rem = pitch - (pixelsPerLine * bytesPerPixel);
  
  switch(bytesPerPixel){
  case 1:
    for(i = 0; i < h; i++){
      src8 = img->pixels + ( (i+1) * pitch) - rem - bytesPerPixel;
      dest8 = mirror->pixels + (i * pitch);
      for(j = 0; j < pixelsPerLine; j++, dest8++, src8--){
	*dest8 = *src8;
      }
    }
    break;
  case 2:
    break;
  case 3:
    break;
  case 4:
    /*
    for(i = 0; i < h; i++){
      src32 = img->pixels + ( (i+1) * pitch) - rem - bytesPerPixel;
      dest32 = mirror->pixels + (i * pitch);
      for(j = 0; j < pixelsPerLine; j++, dest32++, src32--){
	*dest32 = *src32;
      }
    }
    */
    break;
  default:
    break;
  }
  
  return mirror;
}

Rect *newRect(Sint16 x, Sint16 y, Uint16 w, Uint16 h){
  Rect *r;

  r = (Rect *) malloc(sizeof(Rect));
  
  r->x = x;
  r->y = y;
  r->w = w;
  r->h = h;

  return r;
}

// General bitblt function
// Doesn't yet use rects
void bltSurface(Surface *src, Rect *srcr, Surface *dst, Rect *dstr){
  int i;
  Uint8 *line;
  Uint16 w, h, spitch, dpitch;

  w = src->w * src->bytesPerPixel;
  h = src->h;
  spitch = src->pitch;
  dpitch = dst->pitch;
  line = src->pixels;

  for(i = 0; i < h; i++){
    //memcpy(dst->pixels, line, w);
    swiFastCopy(line, dst->pixels, w / 4);
    line += spitch;
    dst->pixels += dpitch;
  }
}
