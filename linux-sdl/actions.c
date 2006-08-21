#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"

#include "dungeon.h"

/* Globals */
#include "globals.h"
extern Globals G;

/*********** Internal functions *************/

void takeStep(){

  switch(G.facing){
  case 0:
    G.y--;
    break;
  case 1:
    G.x++;
    break;
  case 2:
    G.y++;
    break;
  case 3:
    G.x--;
    break;
  default:
    break;
  }

}

/************ Public functions **************/

void turnLeft(){
  if(G.facing == 0) 
    G.facing = 3;
  else
    G.facing -= 1;
}

void turnRight(){
  G.facing = (G.facing +1) % 4;
}

void stepForward(){
  Uint8 tile = lookupTileAhead(G.x, G.y, G.facing, CUR_MAP_DATA);
  Uint8 tileType = tileGetType(tile);

  //printf("tile ahead = %d\n", tileType);

  if(tileType == 0){

  }
  else {
    takeStep();
  }
  
}

void stepBack(){

}


