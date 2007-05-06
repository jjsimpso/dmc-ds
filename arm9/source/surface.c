#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dmc.h"

#include "surface.h"

// Defines for span algorithm
#define SPAN_STATE_OPAQUE  0
#define SPAN_STATE_TRANS   1
#define SPAN_STATE_SKIP_TRANS   2
#define SPAN_STATE_SKIP_OPAQUE   3

// Static functions
static void findSpans(Surface *s);
static void copyPixels(Uint8 *src_line, Uint8 *dst_line, Uint16 h, Uint16 bpr, Uint16 spitch, Uint16 dpitch);
static void copyPixelSpans(Uint8 *src_line, Uint8 *dst_line, Uint16 h, Uint16 bpr, Uint16 spitch, Uint16 dpitch,
			   Uint8 **spans, Uint16 x_offset);

/*
  Create a new surface
*/
Surface *newSurf(Uint16 w, Uint16 h, Uint8 bytesPerPixel, Uint8 alphaColor){
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
  s->alphaColor = alphaColor;

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
  s->pitch = s->bpr + s->padbytes;
  s->pixels = (Uint8 *)malloc(h * s->pitch);

  if(s->pixels == NULL){
    DEBUGF(1,("newSurf: Failed to malloc pixels\n"));    
    /* put this back later
    free(s);
    return NULL;
    */
  }

  return s;
}

Surface *newHWSurf(Uint16 w, Uint16 h, Uint8 bytesPerPixel, Uint16 pitch, Uint8 *data){
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

  // Ensure that the byte width of a row falls on a word boundary
  if(s->bpr % 4){
    DEBUGF(1,("newHWSurf: rows must be aligned\n"));
    return NULL;
  }
  s->rem = 0;
  s->padbytes = 0;

  s->pitch = pitch;
  s->pixels = data;

  return s;
}

// Creates a surface from a C4 image.
// if alphaColor == 0, then there isn't a transparent color in the original 
//   C4 image.  Function is currently ignoring alphaColor.
Surface *newSurfFromC4(C4Img *img, Uint8 alphaColor){
  Surface *s;

  s = newSurf(img->w, img->h, 1, alphaColor);
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

// Creates a surface from a C8 image.
// if alphaColor == 0, then there isn't a transparent color in the original 
//   C8 image
Surface *newSurfFromC8(C8Img *img, Uint8 alphaColor){
  Surface *s;

  s = newSurf(img->w, img->h, 1, alphaColor);
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
  
  if(alphaColor == 0) 
    s->spans = NULL;
  else
    findSpans(s);

  return s;
}

// TODO: need to free spans
void freeSurf(Surface *surf){
  if(surf->pixels != NULL)
    free(surf->pixels);

  if(surf != NULL)
    free(surf);
}

// Need to update
Surface *flipSurface(Surface *img){
  int i, j, padbytes, h, pitch;
  Surface *mirror;
  Uint8 *src8, *dest8;
  //Uint32 *src32, *dest32;
  int pixelsPerLine, bytesPerPixel;

  mirror = newSurf(img->w, img->h, img->bpp, img->alphaColor);
  
  bytesPerPixel = img->bpp;
  pixelsPerLine = img->w;
  h = img->h;
  pitch = img->pitch;
  padbytes = img->padbytes;
  
  //printf("w = %d\n", pixelsPerLine);
  //printf("padbytes = %d\n", padbytes);
  //printf("pitch = %d\n", pitch);

  switch(bytesPerPixel){
  case 1:
    for(i = 0; i < h; i++){
      src8 = img->pixels + ( (i+1) * pitch) - padbytes - 1;
      dest8 = mirror->pixels + (i * pitch);
      for(j = 0; j < pixelsPerLine; j++, dest8++, src8--){
	*dest8 = *src8;
      }
    }
    
    // find spans
    if(mirror->alphaColor == 0) 
      mirror->spans = NULL;
    else
      findSpans(mirror);
    
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
static void findSpans(Surface *s){
  int i,j;
  int w, h, pitch;
  Uint8 *pixel_cursor;
  Uint8 span[20];
  Uint8 num_spans, count, state, alphaColor;

  w = s->w;
  h = s->h;
  pitch = s->pitch;
  alphaColor = s->alphaColor;

  s->spans = (Uint8 **) malloc(sizeof(Uint8 *) * h);
  
  for(i = 0; i < h; i++){
    // initialize variables before reading each row
    pixel_cursor = s->pixels + i * pitch;
    num_spans = 0;
    count = 0;
    state = SPAN_STATE_TRANS; // Start assuming there will be transparent pixels

    for(j = 0; j < w; j++){
      if(state == SPAN_STATE_OPAQUE){ // currently reading visible pixels
	if(*pixel_cursor == alphaColor){
	  span[num_spans++] = count;
	  state = SPAN_STATE_TRANS;
	  count = 1;
	}
	else {
	  count++;
	}
      }
      else {
	if(*pixel_cursor == alphaColor){
	  count++;
	}
	else {
	  span[num_spans++] = count;
	  state = SPAN_STATE_OPAQUE;
	  count = 1;
	}
      }
      pixel_cursor++;
    }
    
    // Tidy up the last span
    span[num_spans++] = count;
    
    // After reading a row, allocate memory and store data in the structure
    //if(i < 5) DEBUGF(1,("row %d: %d spans", i, num_spans));
    s->spans[i] = (Uint8 *)malloc(num_spans+1); // save space for terminator
    memcpy(s->spans[i], span, num_spans);
    s->spans[i][num_spans] = 255;               // span list terminator
    //if(i < 5) DEBUGF(1,(", %d ... %d\n", s->spans[i][0], s->spans[i][num_spans-1]));
  }
  
  return;
}

static void copyPixels(Uint8 *src_line, Uint8 *dst_line, Uint16 h, Uint16 bpr, Uint16 spitch, Uint16 dpitch){
  int i;

  for(i = 0; i < h; i++){
    // have to use memcpy because word alignment is not
    // guaranteed
    memcpy(dst_line, src_line, bpr);
    src_line += spitch;
    dst_line += dpitch;
  }

}

static void copyPixelSpans(Uint8 *src_line, Uint8 *dst_line, Uint16 h, Uint16 bpr, Uint16 spitch, Uint16 dpitch,
		    Uint8 **spans, Uint16 x_offset){
  int i, j;
  int state;
  Uint8 *src, *dst;

  for(i = 0; i < h; i++){
    if(x_offset) 
      state = SPAN_STATE_SKIP_TRANS;       // skip spans until we reach the offset
    else
      state = SPAN_STATE_TRANS;    // first span is transparent
    src = src_line;
    dst = dst_line;

    for(j = 0; spans[i][j] != 255; j++){
      if(state == SPAN_STATE_TRANS){
	src += spans[i][j];
	dst += spans[i][j];
	state = SPAN_STATE_OPAQUE;
      }
      else if(state == SPAN_STATE_OPAQUE){
	memcpy(dst, src, spans[i][j]);
	src += spans[i][j];
	dst += spans[i][j];
	state = SPAN_STATE_TRANS;
      }
      else{
	if(spans[i][j] < x_offset){
	  if(state == SPAN_STATE_SKIP_TRANS) 
	    state = SPAN_STATE_SKIP_OPAQUE;  // skipping opaque
	  else
	    state = SPAN_STATE_SKIP_TRANS;  // skipping transparent
	}
	else{
	  if(state == SPAN_STATE_SKIP_TRANS){
	    src += spans[i][j] - x_offset;
	    dst += spans[i][j] - x_offset;
	    state = SPAN_STATE_OPAQUE;
	  }
	  else{
	    memcpy(dst, src, spans[i][j] - x_offset);
	    src += spans[i][j] - x_offset;
	    dst += spans[i][j] - x_offset;
	    state = SPAN_STATE_TRANS;
	  }
	}
      }

    }

    src_line += spitch;
    dst_line += dpitch;
  }

}

// General bitblt function
void bltSurface(Surface *src, Rect *srcr, Surface *dst, Rect *dstr){

  if(srcr == NULL && dstr == NULL){
    // copy entire src image to dst
    if(src->alphaColor)
      copyPixelSpans(src->pixels, dst->pixels, src->h, src->bpr, src->pitch, dst->pitch, src->spans, 0);
    else
      copyPixels(src->pixels, dst->pixels, src->h, src->bpr, src->pitch, dst->pitch);
    
    //printf("bpr = %d\n", src->bpr);
    //printf("padbytes = %d\n", src->padbytes);
    //printf("rem = %d\n", src->rem);

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
    Uint8 *src_start, *dst_start;
    Uint16 bpr, h;
    
    //printf("(%d,%d) to (%d,%d)\n", srcr->x, srcr->y, dstr->x, dstr->y);

    // Check dimensions (no scaling)
    if( (srcr->w != dstr->w) || (srcr->h != dstr->h)){
      DEBUGF(1,("bltSurface: size error"));
      return;
    }
    
    bpr = srcr->w * src->bpp;
    h = srcr->h;
    src_start = src->pixels + (srcr->y * src->pitch) + (srcr->x * src->bpp);
    dst_start = dst->pixels + (dstr->y * dst->pitch) + (dstr->x * dst->bpp);
    
    //printf("dst->pixels = 0x%x\n", dst->pixels);
    //printf("dst_start = 0x%x\n", dst_start);
    //printf("alphaColor = %d\n", src->alphaColor);
    
    if(src->alphaColor)
      copyPixelSpans(src_start, dst_start, h, bpr, src->pitch, dst->pitch, &(src->spans[srcr->y]), srcr->x);
    else
      copyPixels(src_start, dst_start, h, bpr, src->pitch, dst->pitch);
    
  }

}
