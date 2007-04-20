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
  
  findSpans(s);

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

// Find the spans in s and save them in s->spans
void findSpans(Surface *s){
  int i,j;
  int w, h, pitch;
  Uint8 *pixel_cursor;
  Uint8 span[10];
  Uint8 num_spans, count, state;

  w = s->w;
  h = s->h;
  pitch = s->pitch;

  s->spans = (Uint8 **) malloc(sizeof(Uint8 *) * h);
  
  printf("findSpans\n");
  printf("w = %d\n", w);
  printf("h = %d\n", h);
  printf("pitch = %d\n", pitch);
  
  for(i = 0; i < h; i++){
    // initialize variables before reading each row
    pixel_cursor = s->pixels + i * pitch;
    num_spans = 0;
    count = 0;
    state = 1; // Start assuming there will be transparent pixels

    for(j = 0; j < w; j++){
      if(state == 0){ // currently reading visible pixels
	if(*pixel_cursor == 0){
	  state = 1;
	  span[num_spans++] = count;
	  count = 0;
	}
	else {
	  count++;
	}
      }
      else {
	if(*pixel_cursor == 0){
	  count++;
	}
	else {
	  state = 0;
	  span[num_spans++] = count;
	  count = 0;
	}
      }
      pixel_cursor++;
    }
    
    // Tidy up the last span
    span[num_spans++] = count;
    
    // After reading a row, allocate memory and store data in the structure
    printf("row %d: %d spans", i, num_spans);
    s->spans[i] = (Uint8 *)malloc(num_spans+1); // save space for terminator
    memcpy(s->spans[i], span, num_spans);
    s->spans[i][num_spans] = 255;               // span list terminator
    printf(", %d ... %d\n", s->spans[i][0], s->spans[i][num_spans-1]);
  }
  
  return;
}

void copyPixels(Uint8 *src_line, Uint8 *dst_line, Uint16 h, Uint16 bpr, Uint16 spitch, Uint16 dpitch){
  int i;

  for(i = 0; i < h; i++){
    // have to use memcpy because word alignment is not
    // guaranteed
    memcpy(dst_line, src_line, bpr);
    src_line += spitch;
    dst_line += dpitch;
  }

}

void copyPixelSpans(Uint8 *src_line, Uint8 *dst_line, Uint16 h, Uint16 bpr, Uint16 spitch, Uint16 dpitch,
		    Uint8 **spans){
  int i, j;
  int state;
  Uint8 *src, *dst;

  for(i = 0; i < h; i++){
    state = 1;
    src = src_line;
    dst = dst_line;

    for(j = 0; spans[i][j] != 255; j++){
      if(state == 1){
	src += spans[i][j];
	dst += spans[i][j];
	state = 0;
      }
      else{
	memcpy(dst, src, spans[i][j]);
	src += spans[i][j];
	dst += spans[i][j];
	state = 1;
      }
    }

    src_line += spitch;
    dst_line += dpitch;
  }

}

// General bitblt function
// Doesn't yet use rects
void bltSurface(Surface *src, Rect *srcr, Surface *dst, Rect *dstr){
  int i;
  Uint8 *src_line, *dst_line;
  Uint16 h, spitch, dpitch;

  if(srcr == NULL && dstr == NULL){
    // copy entire src image to dst
    //copyPixels(src->pixels, dst->pixels, src->h, src->bpr, src->pitch, dst->pitch);
    copyPixelSpans(src->pixels, dst->pixels, src->h, src->bpr, src->pitch, dst->pitch, src->spans);
    
    printf("bpr = %d\n", src->bpr);
    printf("padbytes = %d\n", src->padbytes);
    printf("rem = %d\n", src->rem);
    printf("src_line = 0x%x\n", src_line);
    printf("dst_line = 0x%x\n", dst_line);

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