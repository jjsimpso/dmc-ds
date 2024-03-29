#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ARM9

#include <nds.h>			// include your ndslib
#include <gbfs.h>			// filesystem functions
#include "dmc.h"

extern const GBFS_FILE  data_gbfs;
#else
#include "SDL.h"
SDL_Surface *screen;        /* Main screen surface */
Uint32 bg_color;            /* background color */
#endif

#include "graphics.h"


GfxDat *readGfxDat(char *filename){
  FILE *gfile;
  GfxDat *gfxdat;

#ifdef ARM9
  gfile = malloc(sizeof(GBFS_FD));
  gfile->data = gbfs_get_obj(&data_gbfs, filename, NULL);
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

/*
  This is a utility function to create an index to the graphics.dat
  data file.  The index is a simple array of ints pointing to the
  offsets of each file in graphics.dat.

  The index will exist as a separate file on the filesystem.
*/
void writeGfxNdx(GfxDat *gfxdat, char *filename){
  FILE *ndxfile;
  int i, total;

  /* header size */
  total = 8;
  total += (gfxdat->numItems - 1) * 2;

  /* create the index file */
  ndxfile = fopen(filename, "w+");
  if(ndxfile == NULL){
    fprintf(stderr, "Error creating %s\n", filename);
    return;
  }

  /* write the offset of the first file */
  fwrite(&total, 4, 1, ndxfile);
  total += gfxdat->size1;
  
  /* write the offsets of the rest of the files */
  for(i = 0; i < gfxdat->numItems; i++){
    fwrite(&total, 4, 1, ndxfile);
    total += gfxdat->size[i];
  }


  fprintf(stdout, "final size = %d\n", total);

  fclose(ndxfile);

  return;
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
Uint8 readNextNibble(Uint8 *byte, int *flag, FILE *file){
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
Uint8 *procPixelCom(Uint8 pixel, Uint8 *off){
  *off = pixel;
  off++;
  return off;
}

Uint8 *procProjectCom(Uint16 command, Uint8 *off, int ctype){
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

void setColor(SDL_Color *colors, Uint8 i, Uint8 r, Uint8 g, Uint8 b){
  colors[i].r = r;
  colors[i].g = g;
  colors[i].b = b;
}

void copyPalette(Uint8 pal[][3], SDL_Color *colors, int start, int num){
  int i;
  
  for(i = start; i < num; i++){
    colors[i].r = pal[i][0];
    colors[i].g = pal[i][1];
    colors[i].b = pal[i][2];    
  }
}

Uint8 initSDL(Uint16 width, Uint16 height, Uint8 bits){
  SDL_Color colors[256];

  if ( SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0 ) {
    fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
    return 0;
  }

  SDL_WM_SetCaption("Dungeon Master Clone", "dmc");
  
  /* create the screen surface */
  screen = SDL_SetVideoMode(width, height, bits, SDL_SWSURFACE | SDL_DOUBLEBUF);
  if ( screen == NULL ) {
    fprintf(stderr,"Couldn't set %dx%d video mode: %s\n", width, height, SDL_GetError());
    exit(3);
  }

  /* set the background color using the proper pixel format */
  bg_color = SDL_MapRGB(screen->format, 0, 0, 0);
  
  /* Set the palette, if one exists */
  if ( screen->format->palette ) {
    copyPalette(DefaultPalette, colors, 0, 256);
    SDL_SetColors(screen, colors, 0, 256);
  }

  return 1;
}

void mainLoop(){
  SDL_Event event;
  
  while(1){
    if (SDL_WaitEvent(&event) == 0){
      
    }
    else{
      if (event.type == SDL_KEYDOWN){
	/* break from the main loop on a quit event */
	SDL_Quit();
      }
    }
    /* use this to delay */
    /*
    SDL_Delay(time_left());
    G.next_time += TICK_INTERVAL;
    */
  }
  
  return;
}

int main(int argc, char **argv){
  FILE *gfxndx;
  C8Img *img;
  //C4Img *img;
  Uint8 *dest, *src;
  SDL_Color *color;
  int i;

#if 0  /* This is code to create the graphics.ndx file */
  GfxDat *gfxdat;

  gfxdat = readGfxDat("data/dm2/GRAPHICS.DAT");

  if(gfxdat == NULL){
    fprintf(stderr, "Error loading graphics.dat file\n");
    SDL_Quit();
    exit(2);    
  }
  
  fprintf(stdout, "num items = %d\n", gfxdat->numItems);
  fprintf(stdout, "size of first item = %d\n", gfxdat->size1);
  fprintf(stdout, "size of second item = %d\n", gfxdat->size[0]);


  writeGfxNdx(gfxdat, "graphics.ndx");

  return 0;
#else
  FILE *gfxdat;
#endif

  if(initSDL(320, 200, 8) == 0)
    exit(1);

  gfxdat = fopen("data/dm2/GRAPHICS.DAT", "r");
  if(gfxdat == NULL){
    fprintf(stderr, "Error loading graphics.dat file\n");
    SDL_Quit();
    exit(2);
  }

  gfxndx = fopen("graphics.ndx", "r");
  if(gfxndx == NULL){
    fprintf(stderr, "Error loading graphics.ndx file\n");
    SDL_Quit();
    exit(2);
  }

  readFourBitPal(gfxdat, gfxndx, DM2FourBitPalMap);

  //img = loadC4Img(gfxdat, gfxndx, 3);
  img = loadC8Img(gfxdat, gfxndx, 2167);
  //img = loadC8Img(gfxdat, gfxndx, 175);

  printf("bpp = %d\n", screen->format->BitsPerPixel);
  printf("pitch = %d\n", screen->pitch);
  SDL_LockSurface(screen);
  src = img->pixels;
  dest = screen->pixels;

#if 0
  /* print palette */
  for(i = 0; i < 256; i++){
    color = screen->format->palette->colors + i;
    printf("Pixel Color-> Red: %d, Green: %d, Blue: %d. Index: %d\n",
	   color->r, color->g, color->b, i); 
  }
#endif

  /* test blit */
  for(i = 0; i < img->h; i++){
    memcpy(dest, src, img->w);
    dest += screen->pitch;
    src += img->w;
  }
  SDL_UnlockSurface(screen);
  SDL_Flip(screen);

  mainLoop();
  
  SDL_WaitEvent(NULL);
  SDL_Quit();
  fclose(gfxndx);

  return 0;
}
