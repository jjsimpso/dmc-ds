#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nds.h>			// include your ndslib
#include "filesystem.h"
#include "dmc.h"

#include "graphics.h"


GfxDat *readGfxDat(char *filename){
  FILE *gfile;
  GfxDat *gfxdat;

#ifdef ARM9
  gfile = malloc(sizeof(GBFS_FD));
  gfile->data = gbfs_get_obj(gbfs_file, filename, NULL);
  if(gfile->data == NULL){
    return NULL;
  }
  gfile->pos = 0;
#else
  gfile = fopen(filename, "r");
  if(gfile == NULL){
    fprintf(stderr, "Error opening %s\n", filename);
    return NULL;
  }
#endif

  gfxdat = (GfxDat *) malloc(sizeof(GfxDat));
  if(gfxdat != NULL){
    gfxdat->gfile = gfile;
  }
  else {
    fprintf(stderr, "Error allocating GfxDat struct\n");
    return NULL;
  }

  /* skip the file signature */
  fseek(gfile, 2, SEEK_SET);

  /* read header info */
  fread(&gfxdat->numItems, 2, 1, gfile);
  fread(&gfxdat->size1, 4, 1, gfile);

  /* read data sizes */
  gfxdat->size = malloc( (gfxdat->numItems - 1) * 2);
  fread(gfxdat->size, 2, (gfxdat->numItems - 1), gfile);


  return gfxdat;
}

void readGfxNdx(){

}

void readFourBitPal(FILE *gfxdat, FILE *gfxndx, Uint8 *palmap){
  int offset;

  /* read the offset from gfxndx
     16 color palette map is the second entry */
  fseek(gfxndx, 4, SEEK_SET);
  fread(&offset, 4, 1, gfxndx);

  /* read the palette map from graphics.dat */
  fseek(gfxdat, offset, SEEK_SET);
  fread(palmap, 1, 16, gfxdat);

#if 1
 {
   int i;
   for(i = 0; i < 16; i++){
     printf("pal[%d] = %d\n", i, palmap[i]);
   }
 }
#endif
}

/*
  Utility function to get the next nibble out of a file.
  Used by the C4 image algorithm.

  Requires two variables to be used to maintain state, since filesystem
  reads must be at least one byte.
*/
static Uint8 readNextNibble(Uint8 *byte, int *flag, FILE *file){
  Uint8 nibble;

  if(*flag == 0){
    fread(byte, 1, 1, file);
    nibble = *byte & 0xf0;
    *byte = *byte << 4;
    *flag = 1;
  }
  else if(*flag == 1){
    nibble = *byte & 0xf0;
    *byte = 0;
    *flag = 0;
  }

  return (nibble >> 4);
}

C4Img *loadC4Img(FILE *gfxdat, FILE *gfxndx, int file_num){
  int offset, end, tmp, numpix, num, i;
  int readState = 0;
  Uint8 byte, ctl, nib[7], color;
  Uint8 fourBitPalMap[16];
  Uint8 *pixoff, *prevline;
  C4Img *img;
  
  img = (C4Img *) malloc(sizeof(C4Img));
  
  /* get offset and end from gfxndx */
  fseek(gfxndx, file_num * 4, SEEK_SET);
  fread(&offset, 4, 1, gfxndx);
  fread(&end, 4, 1, gfxndx);

  /* read palatte data from end of image data */
  fseek(gfxdat, end - 16, SEEK_SET);
  fread(&fourBitPalMap, 1, 16, gfxdat);

  /* read header data from gfxdat */
  fseek(gfxdat, offset, SEEK_SET);
  fread(&img->w, 2, 1, gfxdat);
  fread(&img->h, 2, 1, gfxdat);
  fread(&tmp, 3, 1, gfxdat);
  
  img->pal[0] = (tmp & 0xf0) >> 4;
  img->pal[1] =  tmp & 0x0f;
  img->pal[2] = (tmp & 0xf000) >> 12;
  img->pal[3] = (tmp & 0x0f00) >> 8;
  img->pal[4] = (tmp & 0xf00000) >> 20;
  img->pal[5] = (tmp & 0x0f0000) >> 16;
  
  /* print out the header info */
  fprintf(stdout, "off = %d\n", offset);
  fprintf(stdout, "w = %d\n", img->w);
  fprintf(stdout, "h = %d\n", img->h);
  for(i = 0; i < 6; i++){
    fprintf(stdout, "pal[%d] = %d\n", i, img->pal[i]);
  }
  
  img->pal[0] = fourBitPalMap[img->pal[0]];
  img->pal[1] = fourBitPalMap[img->pal[1]];
  img->pal[2] = fourBitPalMap[img->pal[2]];
  img->pal[3] = fourBitPalMap[img->pal[3]];
  img->pal[4] = fourBitPalMap[img->pal[4]];
  img->pal[5] = fourBitPalMap[img->pal[5]];
  
  numpix = img->w * img->h;
  img->pixels = (Uint8 *) malloc(numpix);
  
  pixoff = img->pixels;
  while(numpix > 0){
    ctl = readNextNibble(&byte, &readState, gfxdat);

    /* Determine color to add */
    if( (ctl & 0x7) <= 5){
      color = img->pal[ctl & 0x7];
      //printf("color %d at (%d,%d)\n", (ctl & 0x7), ((pixoff - img->pixels) % img->w), 
      //       ((pixoff - img->pixels) / img->w));
    }
    else if( (ctl & 0x7) == 7){
      color = readNextNibble(&byte, &readState, gfxdat);
      //printf("color %d at (%d,%d)\n", color, ((pixoff - img->pixels) % img->w), 
      //       ((pixoff - img->pixels) / img->w));
      color = fourBitPalMap[color];
    }
    else{
      color = 0x10;
    }
    
    /* Determine number of pixels to add*/
    if(ctl & 0x8){
      nib[0] = readNextNibble(&byte, &readState, gfxdat);
      if(nib[0] < 0xf)
	num = nib[0] + 2;
      else {
	nib[1] = readNextNibble(&byte, &readState, gfxdat);
	if(nib[1] < 0xf){
	  nib[2] = readNextNibble(&byte, &readState, gfxdat);
	  num = 17 + (nib[1] * 16) + nib[2];
	}
	else {
	  nib[2] = readNextNibble(&byte, &readState, gfxdat);
	  if(nib[2] < 0xf){
	    num = nib[2] + 257;
	  }
	  else {
	    nib[3] = readNextNibble(&byte, &readState, gfxdat);
	    nib[4] = readNextNibble(&byte, &readState, gfxdat);
	    nib[5] = readNextNibble(&byte, &readState, gfxdat);
	    nib[6] = readNextNibble(&byte, &readState, gfxdat);
	    num = (nib[3] * 4096) + (nib[4] * 256) + (nib[5] * 16) + nib[6];
	  }
	}
      }
    }
    else
      num = 1;
    //printf("num = %d\n", num);
    
    if(num != 1){
      if(color == 0x10){
	prevline = pixoff - img->w;
#if 0
	for(i = 0; i < num; i++){
	  *(pixoff + i) = *prevline++;
	}
#else
	memcpy(pixoff, prevline, num);	
#endif
	printf("110: %d\n", num);
      }
      else {
	memset(pixoff, color, num);
      }
    }
    else {
      if(color == 0x10)
	*pixoff = *(pixoff - img->w);
      else
	*pixoff = color;
    }
    
    numpix -= num;
    pixoff += num;
  }

  printf("bytes read = %d\n", (ftell(gfxdat)) - offset);

  return img;
}

/* 
   The 8bit compressed image format (C8) consists of two basic commands:
   pixel commands and projection commands.
*/
static Uint8 *procPixelCom(Uint8 pixel, Uint8 *off){
  *off = pixel;
  off++;
  return off;
}

static Uint8 *procProjectCom(Uint16 command, Uint8 *off, int ctype){
  Uint8 *dst, *src;
  Uint16 neg_off;
  Uint8 len;
  int i;
  
  if(ctype == 2){
    len     = command & 0x000f;
    neg_off = command >> 4;
  }
  else{
    len     = command & 0x001f;
    neg_off = command >> 5;
  }
  len += 3;

  dst = off;
  src = off - neg_off;
  for(i = 0; i < len; i++) *dst++ = *src++;

  off += len;

  //printf("negoff = %d, len = %d\n", neg_off, len);

  return off;
}

C8Img *loadC8Img(FILE *gfxdat, FILE *gfxndx, int file_num){
  int offset, tmp, i, numpix;
  Uint8 control, pixel;
  Uint16 command;
  Uint8 *pixoff;
  C8Img *img;

  img = (C8Img *) malloc(sizeof(C8Img));

  /* get offset from gfxndx */
  fseek(gfxndx, file_num * 4, SEEK_SET);
  fread(&offset, 4, 1, gfxndx);
  
  /* read data from gfxdat */
  fseek(gfxdat, offset, SEEK_SET);
  fread(&img->w, 2, 1, gfxdat);
  fread(&img->h, 2, 1, gfxdat);
  fread(&img->xoff, 1, 1, gfxdat);
  fread(&img->yoff, 1, 1, gfxdat);
  fread(&img->ctype, 1, 1, gfxdat);
  
  // read zero byte
  fread(&tmp, 1, 1, gfxdat);

  img->w &= 0x03ff;
  img->h &= 0x03ff;
  numpix = img->w * img->h;
  img->pixels = (Uint8 *) malloc(numpix);
  
  pixoff = img->pixels;
  while( (pixoff - img->pixels) < numpix){
    fread(&control, 1, 1, gfxdat);

    for(i = 0; ((i < 8) && ((pixoff - img->pixels) < numpix)); i++){
      if(control & 0x1){
	fread(&pixel, 1, 1, gfxdat);
	pixoff = procPixelCom(pixel, pixoff);
      }
      else{
	fread(&command, 2, 1, gfxdat);
	pixoff = procProjectCom(command, pixoff, img->ctype);
      }
      control = control >> 1;
      //printf("pixoff = %d\n", pixoff - img->pixels);
    }
  }
  
  fprintf(stdout, "off = %d\n", offset);
  fprintf(stdout, "w = %d\n", img->w);
  fprintf(stdout, "h = %d\n", img->h);
  fprintf(stdout, "numpixels = %d\n", pixoff - img->pixels);

  return img;
}

void copyPalette(Uint8 pal[][3], Uint16 *dest, Uint8 start, Uint8 num){
  int i;
  
  for(i = start; i < num; i++){

  }
}



