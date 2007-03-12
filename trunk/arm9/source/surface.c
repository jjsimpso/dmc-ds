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
  Uint8 rem;

  s = (Surface *)malloc(sizeof(Surface));
  if(s == NULL){
    DEBUGF(1,("newSurf: Failed to malloc\n"));
    return NULL;
  }

  s->w = w;
  s->h = h;
  s->bpp = bytesPerPixel;
  s->bpr = w * bytesPerPixel;

  // Ensure that the byte width of a row falls on a word boundary
  if((rem = s->bpr % 4))
    s->bpr += 4 - rem;

  // Pitch is set to either the bytes per row or a special value,
  // such as the width of a hardware surface
  if(pitch == 0){
    s->pitch = s->bpr;
    s->pixels = (Uint8 *)malloc(h * s->bpr);
  }
  else {
    s->pitch = pitch;
    printf("shouldn't be here\n");
    // place image on a hardware surface
    //s->pixels = (Uint8 *)somefunc();
  }

  if(s->pixels == NULL){
    DEBUGF(1,("newSurf: Failed to malloc pixels\n"));    
    /* put this back later
    free(s);
    return NULL;
    */
  }

  return s;
}

Surface *newSurfFromC4(C4Img *img){
  Surface *s;

  s = newSurf(img->w, img->h, 1, 0);
  if(s == NULL)
    return NULL;

  if(s->w == s->bpr)
    memcpy(s->pixels, img->pixels, img->w * img->h);
  else {
    // copy line by line
    Uint8 *src, *dst;
    Uint8 rem;
    Uint16 spitch, dpitch, i;

    src = img->pixels;
    dst = s->pixels;
    spitch = img->w;
    dpitch = s->pitch;
    rem = s->bpr - img->w;
    for(i = 0; i < img->h; i++){
      // Copy row data
      memcpy(dst, src, spitch);

      // Fill in the remaining bytes with the transparent value (index 0)
      memset(dst + spitch, 0, rem);

      src += spitch;
      dst += dpitch;
    }
  }

  return s;
}

Surface *newSurfFromC8(C8Img *img){
  Surface *s;

  s = newSurf(img->w, img->h, 1, 0);
  if(s == NULL)
    return NULL;

  if(s->w == s->bpr)
    memcpy(s->pixels, img->pixels, img->w * img->h);
  else {
    // copy line by line
    Uint8 *src, *dst;
    Uint8 rem;
    Uint16 spitch, dpitch, i;

    src = img->pixels;
    dst = s->pixels;
    spitch = img->w;
    dpitch = s->pitch;
    rem = s->bpr - img->w;
    for(i = 0; i < img->h; i++){
      // Copy row data
      memcpy(dst, src, spitch);

      // Fill in the remaining bytes with the transparent value (index 0)
      memset(dst + spitch, 0, rem);

      src += spitch;
      dst += dpitch;
    }    
  }

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

  mirror = newSurf(img->w, img->h, img->bpp, 0);
  
  bytesPerPixel = img->bpp;
  pixelsPerLine = img->w;
  h = img->h;
  pitch = img->pitch;
  
  rem = img->bpr - (pixelsPerLine * bytesPerPixel);
  
  switch(bytesPerPixel){
  case 1:
    // i should rewrite this to use word copies
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
  
  if(r == NULL){
    DEBUGF(1,("newRect: Failed to malloc\n"));
    return NULL;
  }

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

  w = src->bpr;
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
