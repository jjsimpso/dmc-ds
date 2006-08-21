#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "SDL.h"
#include "SDL_timer.h"
#include "SDL_image.h"

#include "data.h"
#include "dungeon.h"

/* Globals */
#include "globals.h"
extern Globals G;

/*
  obj_orient { 0 = W-E, 1 = N-S }
  facing { 0=N, 1=E, 2=S, 3=W }

  returns { 0 = perpendicular, 1 = parallel}
*/
int findOrientation(int obj_orient, int facing){
  if( (obj_orient && (G.facing == 0 || G.facing == 2)) ||
      (!obj_orient && (G.facing == 1 || G.facing == 3)))
    return 1; // parallel orientation

  // perpendicular orientation
  return 0;
}

void drawTitle(SDL_Surface *surf){
  SDL_BlitSurface(G.misc, NULL, surf, NULL);
  SDL_Flip(surf);
}

void drawFloor(SDL_Surface *surf){
  SDL_BlitSurface(G.wall_gfx.floor, G.wall_gfx.flrRect[0], 
		  surf, G.wall_gfx.flrRect[1]);
}

void drawCeiling(SDL_Surface *surf){
  SDL_BlitSurface(G.wall_gfx.ceiling, G.wall_gfx.clngRect[0], 
		  surf, G.wall_gfx.clngRect[1]);
}

void drawCell(SDL_Surface *img, SDL_Surface *surf, SDL_Rect *src, SDL_Rect *dst){
  if(img != NULL)
    SDL_BlitSurface(img, src, surf, dst);
}

void drawView(int x, int y, int facing, SDL_Surface *surf){
  Uint8 tile, i, tileType;
  int tileError;

  /* clear the screen using the background color */
  SDL_FillRect(surf, NULL, G.bg_color);

  drawCeiling(surf);
  drawFloor(surf);

  for(i = 0; i < NUM_CELLS; i++) {
    tileError = lookupTile(i, x, y, facing, CUR_MAP_DATA, &tile);
    
    if(tileError == 0) {
      tileType = tileGetType(tile);
      
      switch(tileType){
      case 0: /* Wall */
      case 6: /* Trick Wall */
	drawCell(G.wall_gfx.cell[i],  surf, G.wall_gfx.cellRect[i][0], G.wall_gfx.cellRect[i][1]);
	break;
      case 2: /* Pit */
	if(TILEATTR_PIT_OPEN(tile)){
	  drawCell(G.pit_gfx.cell[i],  surf, G.pit_gfx.cellRect[i][0], G.pit_gfx.cellRect[i][1]);
	}
	break;
      case 3: /* Stairs */
	if(TILEATTR_STAIRS_DIR(tile)){ 
	  // Stairs going up
	  if(findOrientation(TILEATTR_STAIRS_ORIENT(tile), G.facing))
	    drawCell(G.stair_para_up_gfx.cell[i],  surf, G.stair_para_up_gfx.cellRect[i][0], G.stair_para_up_gfx.cellRect[i][1]);
	  else
	    drawCell(G.stair_perp_up_gfx.cell[i],  surf, G.stair_perp_up_gfx.cellRect[i][0], G.stair_perp_up_gfx.cellRect[i][1]);
	}
	else {
	  // Stairs going down
	  if(findOrientation(TILEATTR_STAIRS_ORIENT(tile), G.facing))
	    drawCell(G.stair_para_down_gfx.cell[i],  surf, G.stair_para_down_gfx.cellRect[i][0], G.stair_para_down_gfx.cellRect[i][1]);
	  else
	    drawCell(G.stair_perp_down_gfx.cell[i],  surf, G.stair_perp_down_gfx.cellRect[i][0], G.stair_perp_down_gfx.cellRect[i][1]);
	}
	break;
      case 4: /* Door */
	if(findOrientation(TILEATTR_DOOR_ORIENT(tile), G.facing)) {
	  // draw doorway
	  drawCell(G.doorway_para_ls_gfx.cell[i], surf, G.doorway_para_ls_gfx.cellRect[i][0], G.doorway_para_ls_gfx.cellRect[i][1]);
	  drawCell(G.doorway_para_rs_gfx.cell[i], surf, G.doorway_para_rs_gfx.cellRect[i][0], G.doorway_para_rs_gfx.cellRect[i][1]);
	}
	else {
	  // draw doorway
	  drawCell(G.doorway_perp_gfx.cell[i], surf, G.doorway_perp_gfx.cellRect[i][0], G.doorway_perp_gfx.cellRect[i][1]);
	}
	break;
      default: /* Floor=1, Teleporter=5, ExitMap=7 */
	break;
      }
    }
  }


  SDL_Flip(surf);
}


void drawTest(int x, int y, Uint8 *cell, SDL_Surface *surf){
  Uint8 i, tileType;

  /* clear the screen using the background color */
  SDL_FillRect(surf, NULL, G.bg_color);

  drawCeiling(surf);
  drawFloor(surf);

  for(i = 0; i < NUM_CELLS; i++) {
    
    tileType = tileGetType(cell[i]);
    
    switch(tileType){
    case 0: /* Wall */
    case 6: /* Trick Wall */
      drawCell(G.wall_gfx.cell[i],  surf, G.wall_gfx.cellRect[i][0], G.wall_gfx.cellRect[i][1]);
      break;
    case 2: /* Pit */
      drawCell(G.pit_gfx.cell[i],  surf, G.pit_gfx.cellRect[i][0], G.pit_gfx.cellRect[i][1]);
      break;
    case 3: /* Stairs */
      drawCell(G.stair_para_up_gfx.cell[i],  surf, G.stair_para_up_gfx.cellRect[i][0], G.stair_para_up_gfx.cellRect[i][1]);
      break;
    case 4: /* Door */
      drawCell(G.doorway_para_ls_gfx.cell[i],  surf, G.doorway_para_ls_gfx.cellRect[i][0], G.doorway_para_ls_gfx.cellRect[i][1]);
      drawCell(G.doorway_para_rs_gfx.cell[i],  surf, G.doorway_para_rs_gfx.cellRect[i][0], G.doorway_para_rs_gfx.cellRect[i][1]);
      break;
    default: /* Floor=1, Teleporter=5, ExitMap=7 */
      break;
    }
    
  }

  SDL_Flip(surf);
}

  /*
  drawCell(G.wall_gfx.cell[12], surf, G.wall_gfx.cellRect[12][0], G.wall_gfx.cellRect[12][1]);
  drawCell(G.wall_gfx.cell[13], surf, G.wall_gfx.cellRect[13][0], G.wall_gfx.cellRect[13][1]);
  drawCell(G.wall_gfx.cell[14], surf, G.wall_gfx.cellRect[14][0], G.wall_gfx.cellRect[14][1]);
  drawCell(G.wall_gfx.cell[15], surf, G.wall_gfx.cellRect[15][0], G.wall_gfx.cellRect[15][1]);
  drawCell(G.wall_gfx.cell[16], surf, G.wall_gfx.cellRect[16][0], G.wall_gfx.cellRect[16][1]);
  drawCell(G.wall_gfx.cell[17], surf, G.wall_gfx.cellRect[17][0], G.wall_gfx.cellRect[17][1]);
  drawCell(G.wall_gfx.cell[18], surf, G.wall_gfx.cellRect[18][0], G.wall_gfx.cellRect[18][1]);
  drawCell(G.wall_gfx.cell[19], surf, G.wall_gfx.cellRect[19][0], G.wall_gfx.cellRect[19][1]);
  drawCell(G.wall_gfx.cell[20], surf, G.wall_gfx.cellRect[20][0], G.wall_gfx.cellRect[20][1]);
  */
