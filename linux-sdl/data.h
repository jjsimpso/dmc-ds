#ifndef DATA_H_DEFINED
#define DATA_H_DEFINED

#define NUM_CELLS 26

typedef struct {
  SDL_Surface *floor;
  SDL_Surface *ceiling;
  SDL_Surface *cell[NUM_CELLS];

  SDL_Rect *flrRect[2];
  SDL_Rect *clngRect[2];
  SDL_Rect *cellRect[NUM_CELLS][2];
} wallGfx;

typedef struct {
  SDL_Surface *cell[NUM_CELLS];
  SDL_Rect *cellRect[NUM_CELLS][2];
} objGfx;


#define IMG_PATH  "img/"
#define SND_PATH  "snd/"

#define IMG_TITLE_SCREEN "0175.png"

/* Function definitions */
SDL_Rect *newRect(Sint16 x, Sint16 y, Uint16 w, Uint16 h);
SDL_Surface *flipSurface(SDL_Surface *img);
SDL_Surface *readImgFromFS(const char *path, char *file);
int readDataFile(char *path);

int loadUIGfx();
int loadWallGfx(wallGfx *walls, char *dungeon_file);
int loadStairGfx(objGfx *up_stairs, objGfx *down_stairs, char *dungeon_file);
int loadObjGfx(objGfx *pits, char *dungeon_file);

#endif
