//#include <nds.h>			// include your ndslib
//#include <nds/arm9/image.h>		// needed to load pcx files
//#include <nds/arm9/trig_lut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dmc.h"
#include "data.h"

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

void drawTitle(){
  Surface *title;
  //C8Img *title;
  //C4Img *title;

#if 0
  //title = loadC4Img(G.gfxData, G.gfxndx, 3);
  //title = loadC8Img(G.gfxData, G.gfxndx, 175, 0);
  title = loadC8Img(G.gfxData, G.gfxndx, 2167, 10);
  bitBlt8((Uint8 *)BG_GFX, title->pixels, title->w,  title->h);
#else
  title = loadImage(G.gfxData, G.gfxndx, 2167, 0);
  bltSurface(title, NULL, G.DngView, NULL);
#endif
}
