//#include <nds.h>			// include your ndslib
//#include <nds/arm9/image.h>		// needed to load pcx files
//#include <nds/arm9/trig_lut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dmc.h"


/*
  obj_orient { 0 = W-E, 1 = N-S }
  facing { 0=N, 1=E, 2=S, 3=W }

  returns { 0 = perpendicular, 1 = parallel}
*/
int findOrientation(int obj_orient, Uint8 facing){
  if( (obj_orient && (G.facing == 0 || G.facing == 2)) ||
      (!obj_orient && (G.facing == 1 || G.facing == 3)))
    return 1; // parallel orientation

  // perpendicular orientation
  return 0;
}

void swapBGBuffers(Uint8 **front, Uint8 **back){
  Uint8 *temp;

  swiWaitForVBlank();
  
  temp = *front;
  *front = *back;
  *back = temp;
  
  //flip 
  //base is 16KB and screen size is 256x256 (128KB)
  BG2_CR ^= BG_BMP_BASE( 128 / 16 );
}

void drawTitle(){
  Surface *title;
  
  title = loadImage(G.gfxData, G.gfxndx, 175, 0);
  bltSurface(title, NULL, G.DngView, NULL);
}

void drawTest(){
  Surface *img;
#if 0
  //C8Img *title;
  //C4Img *title;

  //title = loadC4Img(G.gfxData, G.gfxndx, 3);
  //title = loadC8Img(G.gfxData, G.gfxndx, 175, 0);
  title = loadC8Img(G.gfxData, G.gfxndx, 2167, 10);
  bitBlt8((Uint8 *)BG_GFX, title->pixels, title->w,  title->h);
#else
  memset((void *)BG_GFX, 43, 512*192);

  bltSurface(G.WallGfx->floor, G.WallGfx->flrRect[0], G.DngView, G.WallGfx->flrRect[1]);
  bltSurface(G.WallGfx->ceiling, G.WallGfx->clngRect[0], G.DngView, G.WallGfx->clngRect[1]);
  //bltSurface(G.WallGfx->cell[22], G.WallGfx->cellRect[22][0], G.DngView, G.WallGfx->cellRect[22][1]);
  bltSurface(G.WallGfx->cell[24], G.WallGfx->cellRect[24][0], G.DngView, G.WallGfx->cellRect[24][1]);
  //img = loadImage(G.gfxData, G.gfxndx, 2165, 10);
  //bltSurface(img, G.WallGfx->cellRect[24][0], G.DngView, G.WallGfx->cellRect[24][1]);
#endif
}

void drawCell(Surface *cell, Surface *surf, Rect *src, Rect *dst){
  if(cell != NULL)
    bltSurface(cell, src, surf, dst);
}

void drawView(Uint16 x, Uint16 y, Uint8 facing, Surface *surf){
  Uint8 tile, i, tileType;
  int tileError;
  
  // clear the screen using the background color
  memset((void *)G.DngView->pixels, 43, 256*192);
  
  // draw the floor and ceiling
  bltSurface(G.WallGfx->floor, G.WallGfx->flrRect[0], G.DngView, G.WallGfx->flrRect[1]);
  bltSurface(G.WallGfx->ceiling, G.WallGfx->clngRect[0], G.DngView, G.WallGfx->clngRect[1]);
  
  for(i = 0; i < NUM_CELLS; i++) {
    tileError = lookupTile(i, x, y, facing, CUR_MAP_DATA, &tile);
    
    if(tileError == 0) {
      tileType = tileGetType(tile);
      
      switch(tileType){
      case 0: /* Wall */
      case 6: /* Trick Wall */
	drawCell(G.WallGfx->cell[i],  surf, G.WallGfx->cellRect[i][0], G.WallGfx->cellRect[i][1]);
	break;
      case 2: /* Pit */
	if(TILEATTR_PIT_OPEN(tile)){
	  //drawCell(G.pit_gfx.cell[i],  surf, G.pit_gfx.cellRect[i][0], G.pit_gfx.cellRect[i][1]);
	}
	break;
      case 3: /* Stairs */
	if(TILEATTR_STAIRS_DIR(tile)){ 
	  // Stairs going up
	  /*
	  if(findOrientation(TILEATTR_STAIRS_ORIENT(tile), G.facing))
	    //drawCell(G.stair_para_up_gfx.cell[i],  surf, G.stair_para_up_gfx.cellRect[i][0], G.stair_para_up_gfx.cellRect[i][1]);
	  else
	    //drawCell(G.stair_perp_up_gfx.cell[i],  surf, G.stair_perp_up_gfx.cellRect[i][0], G.stair_perp_up_gfx.cellRect[i][1]);
	  */
	}
	else {
	  // Stairs going down
	  /*
	  if(findOrientation(TILEATTR_STAIRS_ORIENT(tile), G.facing))
	    drawCell(G.stair_para_down_gfx.cell[i],  surf, G.stair_para_down_gfx.cellRect[i][0], G.stair_para_down_gfx.cellRect[i][1]);
	  else
	    drawCell(G.stair_perp_down_gfx.cell[i],  surf, G.stair_perp_down_gfx.cellRect[i][0], G.stair_perp_down_gfx.cellRect[i][1]);
	  */
	}
	break;
      case 4: /* Door */
	if(findOrientation(TILEATTR_DOOR_ORIENT(tile), G.facing)) {
	  // draw doorway
	  //drawCell(G.doorway_para_ls_gfx.cell[i], surf, G.doorway_para_ls_gfx.cellRect[i][0], G.doorway_para_ls_gfx.cellRect[i][1]);
	  //drawCell(G.doorway_para_rs_gfx.cell[i], surf, G.doorway_para_rs_gfx.cellRect[i][0], G.doorway_para_rs_gfx.cellRect[i][1]);
	}
	else {
	  // draw doorway
	  //drawCell(G.doorway_perp_gfx.cell[i], surf, G.doorway_perp_gfx.cellRect[i][0], G.doorway_perp_gfx.cellRect[i][1]);
	}
	break;
      default: /* Floor=1, Teleporter=5, ExitMap=7 */
	break;
      }
    }
  }

  swapBGBuffers(&G.DVFrontBuffer, &G.DngView->pixels);
}


/*
  DEPRECATED FUNCTIONS
*/
#if 0 
void bitBlt8(Uint8 *dest, Uint8 *src, int w, int h){
  int i;
  Uint8 *line;

  line = src;

  printf("(%d,%d)\n", w, h);
  printf("dest = 0x%x\n", dest);

  for(i = 0; i < h; i++){
    memcpy(dest, line, w);
    dest += 512;
    line += w;
  }

}
#endif
