#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "dmc.h"

#include "data.h"



Surface* loadImage(GfxDat *gfxData, int *gfxndx, int file_num, Uint8 alphaColor){
  Surface *s;
  C8Img *img;

  img = loadC8Img(gfxData, gfxndx, file_num, alphaColor);

  if(img == NULL){
    return NULL;
  }

  s = newSurfFromC8(img, alphaColor);

  freeC8Img(img);

  return s;
}

int loadUIGfx(){

  return 0;
}

Surface* loadDungeonImage(GfxDat *gfxData, int *gfxndx, char *imgname, Uint8 alphaColor){
  Surface *s, *temp;
  C8Img *img;

  if(imgname[0] == '*'){
    img = loadC8Img(gfxData, gfxndx, atoi(imgname+1), alphaColor);
  }
  else {
    img = loadC8Img(gfxData, gfxndx, atoi(imgname), alphaColor);
  }

  if(img == NULL){
    return NULL;
  }

  if(imgname[0] == '*'){
    temp = newSurfFromC8(img, alphaColor);
    s = flipSurface(temp);
    freeSurf(temp);
  }
  else {
    s = newSurfFromC8(img, alphaColor);
  }

  freeC8Img(img);

  return s;
}

int loadWallData(FILE *in, Surface **surf, Rect **src, Rect **dst){
  int conv;
  char line[255];
  char object[255];
  char imgname[255];
  int sx, sy, sw, sh;
  int dx, dy, dw, dh;
  int clr_color;

  fgets(line, 256, in);
  conv = sscanf(line, "%s %s %d %d %d %d %d %d %d %d %d\n", object, imgname, 
		&sx, &sy, &sw, &sh, &dx, &dy, &dw, &dh, &clr_color);

  printf("read %s\n", imgname);
  printf("%d %d %d %d %d %d %d %d %d\n",sx, sy, sw, sh, dx, dy, dw, dh, clr_color);

  if(conv == 11){
    *surf = loadDungeonImage(G.gfxData, G.gfxndx, imgname, clr_color);
    
    *src = newRect(sx, sy, sw, sh);
    *dst = newRect(dx, dy, dw, dh);
  }
  else{
    *surf = NULL;
    DEBUGF(1,("bad line, conv = %d\n", conv));
    return -1;
  }

  return 0;
}

int loadLevelGfx(WallGfx *walls, char *dungeon_file){
  FILE *file;
  int i;

  file = fopen(dungeon_file, "r");
  
  loadWallData(file, &walls->floor, &walls->flrRect[0], &walls->flrRect[1]);
  loadWallData(file, &walls->ceiling, &walls->clngRect[0], &walls->clngRect[1]);
  
  /* walls */
  for(i = 0; i < NUM_CELLS; i++){
    loadWallData(file, &walls->cell[i], &walls->cellRect[i][0], &walls->cellRect[i][1]);
  }

#if 0
  /* up stairs */
  for(i = 0; i < NUM_CELLS; i++){
    loadWallData(file, &up_stairs->cell[i], &up_stairs->cellRect[i][0], &up_stairs->cellRect[i][1]);
  }
  
  /* down stairs */
  for(i = 0; i < NUM_CELLS; i++){
    loadWallData(file, &down_stairs->cell[i], &down_stairs->cellRect[i][0], &down_stairs->cellRect[i][1]);
  }
#endif
  
  fclose(file);
  return 0;
}

int loadObjGfx(ObjGfx *obj, char *objgfx_file){
  FILE *file;
  int i;

  file = fopen(objgfx_file, "r");

  for(i = 0; i < NUM_CELLS; i++){
    loadWallData(file, &obj->cell[i], &obj->cellRect[i][0], &obj->cellRect[i][1]);
  }

  fclose(file);
  return 0;
}


