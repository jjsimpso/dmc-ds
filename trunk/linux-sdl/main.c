#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "SDL.h"
#include "SDL_timer.h"
#include "SDL_image.h"

#include "data.h"
#include "draw.h"
#include "dungeon.h"
#include "actions.h"

#define TICK_INTERVAL    30

/* Globals */
#include "globals.h"
Globals G;


Uint8 initSDL(Uint16 width, Uint16 height, Uint8 bits){
   if ( SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0 ) {
      fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
      return 0;
   }

   SDL_WM_SetCaption("Dungeon Master Clone", "dmc");

   /* create the screen surface */
   G.screen = SDL_SetVideoMode(width, height, bits, SDL_SWSURFACE | SDL_DOUBLEBUF);
   if ( G.screen == NULL ) {
     fprintf(stderr,"Couldn't set %dx%d video mode: %s\n", width, height, SDL_GetError());
     exit(3);
   }

   /* set the background color using the proper pixel format */
   G.bg_color = SDL_MapRGB(G.screen->format, 0, 0, 0);

   /* Set the palette, if one exists */
   /*
   if ( buffers[0]->format->palette ) {
     SDL_SetColors(G.screen, buffers[0]->format->palette->colors, 0, buffers[0]->format->palette->ncolors);
   }
   */

   return 1;
}

void printCoords(){
  printf("(%d,%d)\n", G.x, G.y);
}

Uint32 time_left(void){
  Uint32 now;

  now = SDL_GetTicks();
  if(G.next_time <= now)
    return 0;
  else
    return G.next_time - now;
}

int handleKeyEvents(){
  Uint8 *keys;

  keys = SDL_GetKeyState(NULL);
 
  if(keys[SDLK_q] == SDL_PRESSED)
    return 1;
  else if(keys[SDLK_f] == SDL_PRESSED)
    SDL_WM_ToggleFullScreen(G.screen);
  else if(keys[SDLK_LEFT] == SDL_PRESSED){
    turnLeft();
  }
  else if(keys[SDLK_RIGHT] == SDL_PRESSED){
    turnRight();
  }
  else if(keys[SDLK_UP] == SDL_PRESSED){
    stepForward();
  }
  else if(keys[SDLK_DOWN] == SDL_PRESSED){

  }

  return 0;
}

void mainLoop(){
  SDL_Event event;
  
  G.next_time = SDL_GetTicks() + TICK_INTERVAL;

  /*drawTitle(G.screen);*/
  drawView(G.x, G.y, G.facing, G.screen);

  while(1){
    if (SDL_WaitEvent(&event) == 0){
      
    }
    else{
      if (event.type == SDL_KEYDOWN){
	/* break from the main loop on a quit event */
	if(handleKeyEvents() == 1)
	  break;
      }

      drawView(G.x, G.y, G.facing, G.screen);

    }
    /* use this to delay */
    /*
    SDL_Delay(time_left());
    G.next_time += TICK_INTERVAL;
    */
  }
  
  return;
}

int main(int argc, char *argv[]){
  
  if(argc < 2){
    fprintf(stderr, "dmc <dungeon.dat>\n");
    exit(1);
  }
  
  if(initSDL(320, 200, 0) == 0)
    exit(2);

  /* Load data files */
  if(loadUIGfx()){
    fprintf(stderr, "Error loading UI graphics\n");
    SDL_Quit();
    exit(3);
  }

  /* Load dungeon.dat */
  G.dungeonData = readDngDat(argv[1]);
  if(G.dungeonData == NULL){
    fprintf(stderr, "Error loading dungeon.dat file %s\n", argv[1]);
    SDL_Quit();
    exit(3);    
  }

  if(loadWallGfx(&G.wall_gfx, "dungfx.txt")){
    fprintf(stderr, "Error loading wall graphics\n");
    SDL_Quit();
    exit(3);
  }

  if(loadStairGfx(&G.stair_para_up_gfx, &G.stair_para_down_gfx, "stair_para_gfx.txt")){
    fprintf(stderr, "Error loading north-south stair graphics\n");
    SDL_Quit();
    exit(3);
  }

  if(loadStairGfx(&G.stair_perp_up_gfx, &G.stair_perp_down_gfx, "stair_perp_gfx.txt")){
    fprintf(stderr, "Error loading east-west stair graphics\n");
    SDL_Quit();
    exit(3);
  }

  if(loadObjGfx(&G.pit_gfx, "pitgfx.txt")){
    fprintf(stderr, "Error loading pit graphics\n");
    SDL_Quit();
    exit(3);
  }

  if(loadObjGfx(&G.doorway_para_ls_gfx, "dw_para_ls_gfx.txt")){
    fprintf(stderr, "Error loading north-south doorway graphics\n");
    SDL_Quit();
    exit(3);
  }

  if(loadObjGfx(&G.doorway_para_rs_gfx, "dw_para_rs_gfx.txt")){
    fprintf(stderr, "Error loading north-south doorway graphics\n");
    SDL_Quit();
    exit(3);
  }

  if(loadObjGfx(&G.doorway_perp_gfx, "dw_perp_gfx.txt")){
    fprintf(stderr, "Error loading east-west doorway graphics\n");
    SDL_Quit();
    exit(3);
  }

  G.curMap = 0;
  G.x = 4; //dngGetStartX(G.dungeonData);
  G.y = 15; //dngGetStartY(G.dungeonData);
  G.facing = dngGetStartFacing(G.dungeonData);
 
  printf("starting x = %d\n", G.x);
  printf("starting y = %d\n", G.y);
  printf("starting facing = %d\n", G.facing);

  mainLoop();
  
  SDL_WaitEvent(NULL);
  SDL_Quit();
  
  return 1;
}
