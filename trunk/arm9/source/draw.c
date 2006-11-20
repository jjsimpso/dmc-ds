#include <nds.h>			// include your ndslib
#include <nds/arm9/image.h>		// needed to load pcx files
#include <nds/arm9/trig_lut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filesystem.h"			// filesystem functions
#include "dmc.h"
#include "dungeon.h"
#include "graphics.h"

#include "globals.h"


void bitBlt8(Uint8 *dest, Uint8 *src, int w, int h){
  int i;
  Uint8 *line;

  line = src;
  for(i = 0; i < h; i++){
    memcpy(dest, line, w);
    dest += 512;
    line += w;
  }

}

void drawTitle(){
  C8Img *title;

  //title = loadC4Img(gfxData, gfxndx, 3);
  title = loadC8Img(gfxData, gfxndx, 175);
  bitBlt8((Uint8 *)BG_GFX, title->pixels, title->w,  title->h);
}
