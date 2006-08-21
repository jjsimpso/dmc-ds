#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "SDL.h"
#include "SDL_timer.h"
#include "SDL_image.h"

#include "data.h"

/* Globals */
#include "globals.h"
extern Globals G;

#define LOADGFX(var, path, img) if( (var = readImgFromFS(path, img)) == NULL) return -1;


void dumpPixels(char *filename, SDL_Surface *img){
  FILE *out;
  int size;

  out = fopen(filename, "w");
  size = img->pitch * img->h;
  fwrite(img->pixels, 1, size, out);
  fclose(out);
}

SDL_Surface *readImgFromFS(const char *path, char *file){
  char *fullpath;
  SDL_Surface *surf;
  
  fullpath = (char *) malloc(strlen(path) + strlen(file) + 1);
  sprintf(fullpath, "%s%s", path, file);
  /*printf("%s\n", fullpath);*/
  surf = IMG_Load(fullpath);
  
  free(fullpath);
  return surf;
}


SDL_Surface *flipSurface(SDL_Surface *img){
  int i, j, rem, h, pitch;
  SDL_Surface *mirror;
  Uint8 *src8, *dest8;
  Uint32 *src32, *dest32;
  int pixelsPerLine, bytesPerPixel;

  mirror = SDL_CreateRGBSurface(img->flags, img->w, img->h, img->format->BytesPerPixel * 8,
				0, 0, 0, 0);
  
  if(SDL_LockSurface(mirror)){
    printf("failed to lock mirror\n");
    free(mirror);
    return NULL;
  }
  
  SDL_SetColors(mirror, img->format->palette->colors, 0, img->format->palette->ncolors);

  bytesPerPixel = img->format->BytesPerPixel;
  pixelsPerLine = img->w;
  rem = img->pitch - (img->w * bytesPerPixel);
  
  h = img->h;
  pitch = img->pitch;
  
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
    for(i = 0; i < h; i++){
      src32 = img->pixels + ( (i+1) * pitch) - rem - bytesPerPixel;
      dest32 = mirror->pixels + (i * pitch);
      for(j = 0; j < pixelsPerLine; j++, dest32++, src32--){
	*dest32 = *src32;
      }
    }
    break;
  default:
    break;
  }
  
  SDL_UnlockSurface(mirror);
  return mirror;
}

int loadUIGfx(){
  /*
  if( (G.misc = readImgFromFS(IMG_PATH, IMG_TITLE_SCREEN)) == NULL)
    return -1;
  */
  LOADGFX(G.misc, IMG_PATH, IMG_TITLE_SCREEN);

  return 0;
}


int readDataFile(char *path){

  return 0;
}

SDL_Rect *newRect(Sint16 x, Sint16 y, Uint16 w, Uint16 h){
  SDL_Rect *r;

  r = (SDL_Rect *) malloc(sizeof(SDL_Rect));
  
  r->x = x;
  r->y = y;
  r->w = w;
  r->h = h;

  return r;
}

int loadWallData(FILE *in, SDL_Surface **surf, SDL_Rect **src, SDL_Rect **dst){
  int conv;
  char line[255];
  char object[255];
  char imgname[255];
  int sx, sy, sw, sh;
  int dx, dy, dw, dh;
  int clr_color;
  SDL_Surface *temp;

  fgets(line, 254, in);
  conv = sscanf(line, "%s %s %d %d %d %d %d %d %d %d %d\n", object, imgname, 
		&sx, &sy, &sw, &sh, &dx, &dy, &dw, &dh, &clr_color);

  if(conv == 11){
    if(imgname[0] == '*'){
      LOADGFX(temp, IMG_PATH, imgname + 1);
      *surf = flipSurface(temp);
      SDL_FreeSurface(temp);
    }
    else{
      LOADGFX(*surf, IMG_PATH, imgname);
    }
    *src = newRect(sx, sy, sw, sh);
    *dst = newRect(dx, dy, dw, dh);

    if(clr_color != -1)
      SDL_SetColorKey(*surf, SDL_SRCCOLORKEY, clr_color);
  }
  else{
    *surf = NULL;
    printf("bad line, conv = %d\n", conv);
    return -1;
  }

  return 0;
}

int loadWallGfx(wallGfx *walls, char *dungeon_file){
  FILE *file;
  int i;

  file = fopen(dungeon_file, "r");

  loadWallData(file, &walls->floor, &walls->flrRect[0], &walls->flrRect[1]);
  loadWallData(file, &walls->ceiling, &walls->clngRect[0], &walls->clngRect[1]);

  for(i = 0; i < NUM_CELLS; i++){
    loadWallData(file, &walls->cell[i], &walls->cellRect[i][0], &walls->cellRect[i][1]);
  }

  fclose(file);
  return 0;
}

int loadStairGfx(objGfx *up_stairs, objGfx *down_stairs, char *stairs_file){
  FILE *file;
  int i;

  file = fopen(stairs_file, "r");

  /* up stairs */
  for(i = 0; i < NUM_CELLS; i++){
    loadWallData(file, &up_stairs->cell[i], &up_stairs->cellRect[i][0], &up_stairs->cellRect[i][1]);
  }
  
  /* down stairs */
  for(i = 0; i < NUM_CELLS; i++){
    loadWallData(file, &down_stairs->cell[i], &down_stairs->cellRect[i][0], &down_stairs->cellRect[i][1]);
  }

  fclose(file);

  return 0;
}

int loadObjGfx(objGfx *obj, char *objgfx_file){
  FILE *file;
  int i;

  file = fopen(objgfx_file, "r");

  for(i = 0; i < NUM_CELLS; i++){
    loadWallData(file, &obj->cell[i], &obj->cellRect[i][0], &obj->cellRect[i][1]);
  }

  fclose(file);
  return 0;
}


