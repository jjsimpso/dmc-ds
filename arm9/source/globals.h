#ifndef GLOBALS_H_DEFINED
#define GLOBALS_H_DEFINED

/* Useful macros */
#define IFNN(X,Y) if(X != NULL) Y;

#define CUR_MAP_DATA G.dungeonData->mapData[G.curMap]


typedef struct {
  DngDat *dungeonData;
  GfxDat *gfxData;
  int *gfxndx;
} Globals;

/*
extern Uint8 DefaultPalette[256][3];
extern DngDat *dungeonData;
extern GfxDat *gfxData;
extern int *gfxndx;
*/
extern Globals G;                      //defined in main.c
extern  Uint8 DefaultPalette[256][3];  //defined in defpal.h
extern GBFS_FILE const* gbfs_file;     //defined in main.c





#if 0
typedef struct {
  SDL_Surface *misc;          
  SDL_Surface *screen;        // Main screen surface
  Uint32 bg_color;            // background color
  wallGfx wall_gfx;
  objGfx stair_para_up_gfx;
  objGfx stair_para_down_gfx;
  objGfx stair_perp_up_gfx;
  objGfx stair_perp_down_gfx;
  objGfx pit_gfx;
  objGfx doorway_para_ls_gfx;
  objGfx doorway_para_rs_gfx;
  objGfx doorway_perp_gfx;
  objGfx door0_gfx;
  objGfx door1_gfx;
  objGfx door0_closed_gfx;
  objGfx door1_closed_gfx;
  Uint32 next_time;
  DngDat *dungeonData;
  int curMap, x, y, facing;
  
} Globals;
#endif


#endif
