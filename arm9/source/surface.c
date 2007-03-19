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
  if(s == NULL){
    DEBUGF(1,("newSurf: Failed to malloc\n"));
    return NULL;
  }

  s->w = w;
  s->h = h;
  s->bpp = bytesPerPixel;
  s->bpr = w * bytesPerPixel;

#if 1
  // Ensure that the byte width of a row falls on a word boundary
  if((s->rem = s->bpr % 4))
    s->padbytes = 4 - s->rem;
  else
    s->padbytes = 0;
#else
  // Instead of forcing the byte width of a row to fall on a word
  // boundary, save the remainder.
  s->rem = s->bpr % 4;
#endif

  // Pitch is set to either the bytes per row or a special value,
  // such as the width of a hardware surface
  if(pitch == 0){
    s->pitch = s->bpr + s->padbytes;
    s->pixels = (Uint8 *)malloc(h * s->pitch);
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

  if(s->rem == 0)
    memcpy(s->pixels, img->pixels, img->w * img->h);
  else {
    // copy line by line
    Uint8 *src, *dst;
    Uint8 padbytes;
    Uint16 spitch, dpitch, i;

    src = img->pixels;
    dst = s->pixels;
    spitch = img->w;
    dpitch = s->pitch;
    padbytes = s->padbytes;
    for(i = 0; i < img->h; i++){
      // Copy row data
      memcpy(dst, src, spitch);

      // Fill in the remaining bytes with the transparent value (index 0)
      memset(dst + spitch, 0, padbytes);

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

  if(s->rem == 0)
    memcpy(s->pixels, img->pixels, img->w * img->h);
  else {
    // copy line by line
    Uint8 *src, *dst;
    Uint8 padbytes;
    Uint16 spitch, dpitch, i;

    src = img->pixels;
    dst = s->pixels;
    spitch = img->w;
    dpitch = s->pitch;
    padbytes = s->padbytes;
    for(i = 0; i < img->h; i++){
      // Copy row data
      memcpy(dst, src, spitch);

      // Fill in the remaining bytes with the transparent value (index 0)
      memset(dst + spitch, 0, padbytes);

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

// Need to update
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
  Uint8 rem, wpr;
  Uint8 *src_line, *dst_line;
  Uint16 chunksize, h, spitch, dpitch;

  if(srcr == NULL && dstr == NULL){
    // copy entire src image to dst
    
    h = src->h;
    rem = src->rem;
    chunksize = src->bpr - rem;
    wpr = chunksize / 4;
    spitch = src->pitch;
    dpitch = dst->pitch;
    src_line = src->pixels;
    dst_line = dst->pixels;
    
    printf("rem = %d\n", rem);
    printf("bpr = %d\n", src->bpr);
    printf("padbytes = %d\n", src->padbytes);
    printf("chunksize = %d\n", chunksize);
    printf("wpr = %d\n", wpr);
    printf("src_line = 0x%x\n", src_line);
    printf("dst_line = 0x%x\n", dst_line);

    for(i = 0; i < h; i++){
      //memcpy(dst_line, src_line, src->bpr);
      swiFastCopy(src_line, dst_line, wpr);
      memcpy(dst_line + chunksize, src_line + chunksize, rem);
      src_line += spitch;
      dst_line += dpitch;
    }
    /*
    printf("src[166] = %d\n", src->pixels[166]);
    printf("src[167] = %d\n", src->pixels[167]);
    printf("src[168] = %d\n", src->pixels[168]);
    printf("src[169] = %d\n", src->pixels[169]);

    printf("dst[166] = %d\n", dst->pixels[166]);
    printf("dst[167] = %d\n", dst->pixels[167]);
    printf("dst[168] = %d\n", dst->pixels[168]);
    printf("dst[169] = %d\n", dst->pixels[169]);
    */
    return;
  }
  else if(srcr == NULL){
    // Copy src to dstr

  }
  else if(dstr == NULL){
    // Copy srcr to dst

  }
  else{
    // Copy srcr to dstr
    if( (srcr->w != dstr->w) || (srcr->h != dstr->h)){
      DEBUGF(1,("bltSurface: size error"));
      return;
    }
  }

}
