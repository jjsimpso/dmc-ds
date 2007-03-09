#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#define NUM_CELLS 26

#define IMG_TITLE_SCREEN 175

#include "surface.h"


typedef struct {
  Surface *floor;
  Surface *ceiling;
  Surface *cell[NUM_CELLS];

  Rect *flrRect[2];
  Rect *clngRect[2];
  Rect *cellRect[NUM_CELLS][2];
} WallGfx;

typedef struct {
  Surface *cell[NUM_CELLS];
  Rect *cellRect[NUM_CELLS][2];
} ObjGfx;


/* Function declarations */
Surface* loadImage(GfxDat *gfxData, int *gfxndx, int file_num, Uint8 alphaColor);
int loadUIGfx();
int loadLevelGfx(WallGfx *walls, char *dungeon_file);
int loadObjGfx(ObjGfx *pits, char *dungeon_file);

#endif /* DATA_H_INCLUDED */
