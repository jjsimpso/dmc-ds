#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ARM9

#include <nds.h>			// include your ndslib
#include "filesystem.h"
#include "dmc.h"

#else
#include "SDL.h"
#endif

#include "dungeon.h"

#if 1
#define SWAPBYTES
#endif


/* 
-----------------------------------------------------------
UTILITY FUNCTIONS
-----------------------------------------------------------
*/
void swapBytes(
   int	*intPtr,    /* pointer of words requiring byte swapping */
   unsigned int	sizeBytes   /* number of bytes in array */
){
   int sizeInts, i;
   unsigned char *bytePtr;

   sizeInts = sizeBytes / 4;
   for (i = 0; i < sizeInts; i++){
      bytePtr = (unsigned char *) intPtr;
      *intPtr++ = ((bytePtr[0]<<24) | (bytePtr[1]<<16) | (bytePtr[2]<<8) | bytePtr[3]);
   }
}

/* in this case a word is 16 bits */
void swapWord(Uint16 *wordPtr, int numWords){
  int i;
  unsigned char *bytePtr;

  for(i = 0; i < numWords; i++){
    bytePtr = (unsigned char *) wordPtr;
    *wordPtr++ = (bytePtr[0] << 8) | bytePtr[1];
  }
}

/* 
-----------------------------------------------------------
ACCESSOR FUNCTIONS
-----------------------------------------------------------
*/
Uint8 dngGetStartLevel(DngDat *dng){
  return 0;
}

Uint8 dngGetStartX(DngDat *dng){
  return (dng->header.startingPos & DNGHDR_STARTPOS_X_MASK);
}

Uint8 dngGetStartY(DngDat *dng){
  Uint16 temp = dng->header.startingPos & DNGHDR_STARTPOS_Y_MASK;

  return (temp >> 5);
}

Uint8 dngGetStartFacing(DngDat *dng){
  Uint16 temp = dng->header.startingPos & DNGHDR_STARTPOS_FACING_MASK;

  return (temp >> 10);
}

Uint8 mapGetLevel(DngMap *map){

  return (map->size & DNGMAP_SIZE_LEVEL_MASK);
}

Uint8 mapGetWidth(DngMap *map){
  Uint16 temp = map->size & DNGMAP_SIZE_WIDTH_MASK;

  return (temp >> 6) + 1;
}

Uint8 mapGetHeight(DngMap *map){
  Uint16 temp = map->size & DNGMAP_SIZE_HEIGHT_MASK;

  return (temp >> 11) + 1;
}

Uint8 mapGetNumWallGfx(DngMap *map){

  return (map->numGfx & DNGMAP_NUMGFX_WALLS_MASK);
}

Uint8 mapGetNumRandWallGfx(DngMap *map){
  Uint16 temp = map->numGfx & DNGMAP_NUMGFX_RANDWALLS_MASK;

  return (temp >> 4);
}

Uint8 mapGetNumPadGfx(DngMap *map){
  Uint16 temp = map->numGfx & DNGMAP_NUMGFX_PADS_MASK;

  return (temp >> 8);
}

Uint8 mapGetNumRandPadGfx(DngMap *map){
  Uint16 temp = map->numGfx & DNGMAP_NUMGFX_RANDPADS_MASK;

  return (temp >> 12);
}

Uint8 mapGetNumOrnateGfx(DngMap *map){

  return (map->misc & DNGMAP_MISC_ORNATE_MASK);
}

Uint8 mapGetNumCreatures(DngMap *map){
  Uint16 temp = map->misc & DNGMAP_MISC_CREATURES_MASK;

  return (temp >> 4);
}

Uint8 mapGetDepth(DngMap *map){
  Uint16 temp = map->misc & DNGMAP_MISC_DEPTH_MASK;

  return (temp >> 12);
}

Uint8 mapGetStyle(DngMap *map){
  Uint16 temp = map->doorGfx & DNGMAP_DOORGFX_STYLE_MASK;

  return (temp >> 4);
}

Uint8 mapGetDoor0Gfx(DngMap *map){
  Uint16 temp = map->doorGfx & DNGMAP_DOORGFX_DOOR0GFX_MASK;

  return (temp >> 8);
}

Uint8 mapGetDoorType(DngMap *map){
  Uint16 temp = map->doorGfx & DNGMAP_DOORGFX_DOORTYPE_MASK;

  return (temp >> 12);
}

Uint8 mapGetTile(Uint8 x, Uint8 y, DngMapData *mapData){

  return *(mapData->tiles + (mapData->h * x) + y);
}

Uint8 tileGetType(Uint8 tile){
  Uint8 temp = tile & DNGMAPTILE_TYPE_MASK;

  return (temp >> 5);
}

Uint8 tileGetObjects(Uint8 tile){
  if(tile & DNGMAPTILE_OBJECTS_MASK)
    return 1;
  else
    return 0;
}

Uint8 tileGetAttribs(Uint8 tile){

  return (tile & DNGMAPTILE_ATTRIBS_MASK);
}

Uint8 textGetChar1(Uint16 textWord){
  Uint16 temp = textWord & DNGMAPTEXT_CHAR1;

  return (temp >> 10);
}

Uint8 textGetChar2(Uint16 textWord){
  Uint16 temp = textWord & DNGMAPTEXT_CHAR2;

  return (temp >> 5);
}

Uint8 textGetChar3(Uint16 textWord){

  return (textWord & DNGMAPTEXT_CHAR3);
}


/* 
-----------------------------------------------------------
SETTER FUNCTIONS
-----------------------------------------------------------
*/

Uint8 tileNew(Uint8 type){
  return (type << 5);
}

/* returns a new tile value */
Uint8 tileSetType(Uint8 tile, Uint8 type){
  Uint8 temp = tile & ~DNGMAPTILE_TYPE_MASK;

  return ((type << 5) | temp);
}


/*
-----------------------------------------------------------
SPECIFIC TILE ATTRIBUTE FUNCTIONS
-----------------------------------------------------------
*/

/* for example */
int doorIsOpen(Uint8 tile){

}




/*
-----------------------------------------------------------
MAP NAVIGATION FUNCTIONS
-----------------------------------------------------------
*/

/* 
   Useful Info:
   The origin of each map is the upper left corner.
   Facing values: 0=N, 1=E, 2=S, 3=W
*/

int calcTileIndex(int x, int y, DngMapData *map){
  return x * map->h + y;
}
#define TILE_INDEX(x, y, map) x * map->h + y;

int calcX(int x, int facing, int steps){
  switch(facing){
  case 0:
  case 2:
    break;
  case 1:
    x += steps;
    break;
  case 3:
    x -= steps;
    break;
  }

  return x;
}

int calcY(int y, int facing, int steps){
  switch(facing){
  case 1:
  case 3:
    break;
  case 0:
    y -= steps;
    break;
  case 2:
    y += steps;
    break;
  }

  return y;
}

/* Return the tile value of the tile to move to */
Uint8 lookupTileAhead(int x, int y, int facing, DngMapData *map) {
  int index;

  switch(facing){
  case 0:
    y--;
    break;
  case 1:
    x++;
    break;
  case 2:
    y++;
    break;
  case 3:
    x--;
    break;
  default:
    break;
  }
  
  index = TILE_INDEX(x, y, map);
  
  return map->tiles[index];
}

/*
  Returns: 0 on success, -1 on error
  Side Effects: tile is set to hold the tile data
*/
int lookupTile(int num, int x, int y, int facing, DngMapData *map, Uint8 *tile){
  int index, xsteps, ysteps, temp;

  switch(num){
  case 0:
    xsteps = -2;
    ysteps = -5;
    break;
  case 1:
    xsteps = 2;
    ysteps = -5;
    break;
  case 2:
    xsteps = -2;
    ysteps = -4;
    break;
  case 3:
    xsteps = 2;
    ysteps = -4;
    break;
  case 4:
    xsteps = -1;
    ysteps = -5;
    break;
  case 5:
    xsteps = 1;
    ysteps = -5;
    break;
  case 6:
    xsteps = 0;
    ysteps = -5;
    break;
  case 7:
    xsteps = -1;
    ysteps = -4;
    break;
  case 8:
    xsteps = 1;
    ysteps = -4;
    break;
  case 9:
    xsteps = 0;
    ysteps = -4;
    break;
  case 10:
    xsteps = -2;
    ysteps = -3;
    break;
  case 11:
    xsteps = 2;
    ysteps = -3;
    break;
  case 12:
    xsteps = -1;
    ysteps = -3;
    break;
  case 13:
    xsteps = 1;
    ysteps = -3;
    break;
  case 14:
    xsteps = 0;
    ysteps = -3;
    break;
  case 15:
    xsteps = -2;
    ysteps = -2;
    break;
  case 16:
    xsteps = 2;
    ysteps = -2;
    break;
  case 17:
    xsteps = -1;
    ysteps = -2;
    break;
  case 18:
    xsteps = 1;
    ysteps = -2;
    break;
  case 19:
    xsteps = 0;
    ysteps = -2;
    break;
  case 20:
    xsteps = -1;
    ysteps = -1;
    break;
  case 21:
    xsteps = 1;
    ysteps = -1;
    break;
  case 22:
    xsteps = 0;
    ysteps = -1;
    break;
  case 23:
    xsteps = -1;
    ysteps = 0;
    break;
  case 24:
    xsteps = 1;
    ysteps = 0;
    break;
  case 25:
    xsteps = 0;
    ysteps = 0;
    break;
  default:
    break;
  }

  //printf("%d,%d swaps to ", xsteps, ysteps);

  switch(facing){
  case 0:
    break;
  case 1:
    temp = xsteps;
    xsteps = ysteps * -1;
    ysteps = temp;
    break;
  case 2:
    xsteps *= -1;
    ysteps *= -1;
    break;
  case 3:
    temp = xsteps;
    xsteps = ysteps;
    ysteps = temp * -1;
    break;
  }

  //printf("%d,%d :: ", xsteps, ysteps);
  //printf("(%d,%d) --> ", x, y);

  x += xsteps;
  y += ysteps;

  //printf("(%d,%d)\n", x, y);

  if((x < 0) || (y < 0) || (x >= map->w) || (y >= map->h)) {
    return -1;
  }

  index = TILE_INDEX(x, y, map);
  *tile = map->tiles[index];

  return 0;
}
   



/* 
-----------------------------------------------------------
OUTPUT FUNCTIONS
-----------------------------------------------------------
*/
void printDngHeader(DngHeader *hdr){
  fprintf(stdout, "seed = %d\n", hdr->seed);
  fprintf(stdout, "map data size in bytes = %d\n", hdr->mapDataSizeBytes);
  fprintf(stdout, "num maps = %d\n", hdr->numMaps);
  fprintf(stdout, "text data size in words = %d\n", hdr->textDataSizeWords);
  fprintf(stdout, "starting position = %d\n", hdr->startingPos);
  fprintf(stdout, "object list size in words = %d\n", hdr->objListSizeWords);
  fprintf(stdout, "number of doors = %d\n", hdr->numDoors);
  fprintf(stdout, "number of teleporters = %d\n", hdr->numTeleporters);
  fprintf(stdout, "number of texts = %d\n", hdr->numTexts);
  fprintf(stdout, "number of actuators = %d\n", hdr->numActuators);
  fprintf(stdout, "number of creatures = %d\n", hdr->numCreatures);
  fprintf(stdout, "number of weapons = %d\n", hdr->numWeapons);
  fprintf(stdout, "number of clothes = %d\n", hdr->numClothes);
  fprintf(stdout, "number of scrolls = %d\n", hdr->numScrolls);
  fprintf(stdout, "number of potions = %d\n", hdr->numPotions);
  fprintf(stdout, "number of chests = %d\n", hdr->numChests);
  fprintf(stdout, "number of misc items = %d\n", hdr->numMiscItems);
  fprintf(stdout, "number of missiles = %d\n", hdr->numMissiles);
  fprintf(stdout, "number of clouds = %d\n", hdr->numClouds);

  return;
}

void printDngTextString(Uint8 *text){
  int i = 0;

  while(1) {
    /* damn escape code again... */
    if( (text[i] == DNG_TEXT_TERM) && (text[i-1] != 29) )
      return;

    printf("%c", (text[i] + 'a'));
    i++;
  }
 
}

void printDngText(DngDat *dungeon){
  int i;
  int numTexts = dungeon->header.numTexts;

  for(i = 0; i < numTexts; i++){
    printDngTextString(dungeon->textData[i]);
    printf("\n");
  }

}

void printDngMap(DngMap *map){
  fprintf(stdout, "offset = %d\n", map->offset);
  fprintf(stdout, "xoffset = %d\n", map->xoffset);
  fprintf(stdout, "yoffset = %d\n", map->yoffset);
  fprintf(stdout, "size = 0x%x\n", map->size);
  fprintf(stdout, "numGfx = 0x%x\n", map->numGfx);
  fprintf(stdout, "misc = 0x%x\n", map->misc);
  fprintf(stdout, "doorGfx = 0x%x\n", map->doorGfx);

  return;
}

void printDngMapTiles(DngMapData *map){
  int i, j;
  int w,h;
  int type;

  w = map->w;
  h = map->h;

  for(i = 0; i < h; i++){
    for(j = 0; j < w; j++){
      type = tileGetType(map->tiles[j * h + i]);
      //type = tileGetType(map->tiles[i * h + j]);
      if(type == 0)
	printf("*");
      else if(type == 1)
	printf("-");
      else
	printf("%d", type);
    }
    printf("\n");
  }
}

/* 
-----------------------------------------------------------
READ FUNCTIONS
-----------------------------------------------------------
*/
int readDngHeader(FILE *file, DngHeader *hdr){
  fread(&hdr->seed,              2, 1, file);
  fread(&hdr->mapDataSizeBytes,  2, 1, file);
  fread(&hdr->numMaps,           1, 1, file);
  fread(&hdr->unknown1,          1, 1, file);
  fread(&hdr->textDataSizeWords, 2, 1, file);
  fread(&hdr->startingPos,       2, 1, file);
  fread(&hdr->objListSizeWords,  2, 1, file);
  fread(&hdr->numDoors,          2, 1, file);
  fread(&hdr->numTeleporters,    2, 1, file);
  fread(&hdr->numTexts,          2, 1, file);
  fread(&hdr->numActuators,      2, 1, file);
  fread(&hdr->numCreatures,      2, 1, file);
  fread(&hdr->numWeapons,        2, 1, file);
  fread(&hdr->numClothes,        2, 1, file);
  fread(&hdr->numScrolls,        2, 1, file);
  fread(&hdr->numPotions,        2, 1, file);
  fread(&hdr->numChests,         2, 1, file);
  fread(&hdr->numMiscItems,      2, 1, file);
  fread(&hdr->unknown2,          1, 6, file);
  fread(&hdr->numMissiles,       2, 1, file);
  fread(&hdr->numClouds,         2, 1, file);

#ifdef SWAPBYTES
  swapWord(&hdr->seed,              1);
  swapWord(&hdr->mapDataSizeBytes,  1);
  swapWord(&hdr->textDataSizeWords, 1);
  swapWord(&hdr->startingPos,       1);
  swapWord(&hdr->objListSizeWords,  1);
  swapWord(&hdr->numDoors,          1);
  swapWord(&hdr->numTeleporters,    1);
  swapWord(&hdr->numTexts,          1);
  swapWord(&hdr->numActuators,      1);
  swapWord(&hdr->numCreatures,      1);
  swapWord(&hdr->numWeapons,        1);
  swapWord(&hdr->numClothes,        1);
  swapWord(&hdr->numScrolls,        1);
  swapWord(&hdr->numPotions,        1);
  swapWord(&hdr->numChests,         1);
  swapWord(&hdr->numMiscItems,      1);
  swapWord(&hdr->numMissiles,       1);
  swapWord(&hdr->numClouds,         1);
#endif

  return 0;
}

DngMap *readDngMap(FILE *file){
  DngMap *map;

  map = (DngMap *) malloc(sizeof(DngMap));

  fread(&map->offset,   2, 1, file);
  fread(&map->unknown1, 1, 4, file);
  fread(&map->xoffset,  1, 1, file);
  fread(&map->yoffset,  1, 1, file);
  fread(&map->size,     2, 1, file);
  fread(&map->numGfx,   2, 1, file);
  fread(&map->misc,     2, 1, file);
  fread(&map->doorGfx,  2, 1, file);

#ifdef SWAPBYTES
  swapWord(&map->offset, 1);
  swapWord(&map->size, 1);
  swapWord(&map->numGfx, 1);
  swapWord(&map->misc, 1);
  swapWord(&map->doorGfx, 1);
#endif

  return map;
}

Uint16 *readDngTilesWithObjsIndex(FILE *file, int numColumns){
  Uint16 *tilesWithObjsIndex;

  tilesWithObjsIndex = (Uint16 *) malloc(sizeof(Uint16) * numColumns);

  fread(tilesWithObjsIndex, 2, numColumns, file);
#ifdef SWAPBYTES
  swapWord(tilesWithObjsIndex, numColumns);
#endif

  return tilesWithObjsIndex;
}

Uint16 *readDngObjList(FILE *file, int numWords){
  Uint16 *objList;

  objList = (Uint16 *) malloc(sizeof(Uint16) * numWords);

  fread(objList, 2, numWords, file);
#ifdef SWAPBYTES
  swapWord(objList, numWords);
#endif

  return objList;
}

int strlenDngText(Uint8 *string){
  int i = 0;

  while(string[i] != DNG_TEXT_TERM) i++;

  return i;
}

/* 
   Returns new dest index, or 0 if the end of the string is
   reached.
*/
int parseDngTextWord(Uint16 textWord, Uint8 dest[], int index){
  /* The text format is kind of annoying:
   - 31 is the end-of-string character.
   - 29 is an escape code, where a value of 31 in the next
     character is valid.     
   - My code makes the assumption that a string won't begin
     with a '31'.
   */

  dest[index++] = textGetChar1(textWord);
  if( (dest[index - 1] == 31) && (dest[index - 2] != 29) )
    return 0;

  dest[index++] = textGetChar2(textWord);
  if( (dest[index - 1] == 31) && (dest[index - 2] != 29) )
    return 0;

  dest[index++] = textGetChar3(textWord);
  if( (dest[index - 1] == 31) && (dest[index - 2] != 29) )
    return 0;

  return index;
}

Uint8 *readDngTextString(FILE *file){
  Uint16 textWord;
  Uint8 temp[256];
  Uint8 *string;
  int i = 0;
  int len;

  do {
    fread(&textWord, 2, 1, file);
#ifdef SWAPBYTES
    swapWord(&textWord, 1);
#endif

  } while( (i = parseDngTextWord(textWord, temp, i)) );

  len = strlenDngText(temp) + 1;
  string = (Uint8 *) malloc(len);
  memcpy(string, temp, len);

  return string;
}

Uint8 **readDngTextData(FILE *file, int numTexts){
  Uint8 **textData;
  int i;

  textData = (Uint8 **) malloc(sizeof(Uint8 *) * numTexts);

  for(i = 0; i < numTexts; i++){
    textData[i] = readDngTextString(file);    
  }

  return textData;
}

DngMapData *readDngMapData(FILE *file, DngMap *map){
  int numCreatureGfx, numWallGfx, numPadGfx, numOrnateGfx;
  int numTiles;
  DngMapData *mapData;
  
  mapData = (DngMapData *) malloc(sizeof(DngMapData));
  /*
  mapData->xorg = map->xoffset;
  mapData->yorg = map->yoffset;
  */
  mapData->w = mapGetWidth(map);
  mapData->h = mapGetHeight(map);
  
  numTiles = mapData->w * mapData->h;
  mapData->tiles = (Uint8 *) malloc(numTiles);
  
  fread(mapData->tiles, 1, numTiles, file);

  numCreatureGfx = mapGetNumCreatures(map);
  numWallGfx = mapGetNumWallGfx(map);
  numPadGfx = mapGetNumPadGfx(map);
  numOrnateGfx = mapGetNumOrnateGfx(map);

  fread(mapData->creatureGfx, 1, numCreatureGfx, file);
  fread(mapData->wallGfx,     1, numWallGfx,     file);
  fread(mapData->padGfx,      1, numPadGfx,      file);
  fread(mapData->ornateGfx,   1, numOrnateGfx,   file);

  return mapData;
}

DngDat *readDngDat(char *filename){
  FILE *dfile;
  DngDat *dungeonData;
  int numMaps; 
  int numColumns = 0;
  int i;

#ifdef ARM9
  dfile = malloc(sizeof(GBFS_FD));
  dfile->data = gbfs_get_obj(gbfs_file, filename, NULL);
  if(dfile->data == NULL){
    return NULL;
  }
  dfile->pos = 0;
#else
  dfile = fopen(filename, "r");
  if(dfile == NULL){
    return NULL;
  }
#endif

  dungeonData = (DngDat *) malloc(sizeof(DngDat));
  
  /* read the dungeon.dat header */
  readDngHeader(dfile, &dungeonData->header);
  printDngHeader(&dungeonData->header);

  /* read the dungeon map definition data */
  numMaps = dungeonData->header.numMaps;
  dungeonData->maps = (DngMap **) malloc(sizeof(DngMap *) * numMaps);

  for(i = 0; i < numMaps; i++){
    dungeonData->maps[i] = readDngMap(dfile);
    numColumns += mapGetWidth(dungeonData->maps[i]);
    printf("map %d: level = %d, w = %d, h = %d\n", i, mapGetLevel(dungeonData->maps[i]),
            mapGetWidth(dungeonData->maps[i]), mapGetHeight(dungeonData->maps[i]));
    //printDngMap(dungeonData->maps[i]);
  }

  printf("num columns = %d\n", numColumns);
  printf("current file pos = %d\n", ftell(dfile));
  /* read the index of tiles with objects */
  dungeonData->tilesWithObjsIndex = readDngTilesWithObjsIndex(dfile, numColumns);

  printf("current file pos = %d\n", ftell(dfile));
  /* read list of first objects on each tile */
  dungeonData->objList = readDngObjList(dfile, dungeonData->header.objListSizeWords);

  printf("current file pos = %d\n", ftell(dfile));
  /* read text data */
  dungeonData->textData = readDngTextData(dfile, dungeonData->header.numTexts);

  printf("current file pos = %d\n", ftell(dfile));
  /* skip lists*/
  fseek(dfile, dungeonData->header.numDoors       * (DNGLIST_DOORS_SIZE), SEEK_CUR);
  fseek(dfile, dungeonData->header.numTeleporters * (DNGLIST_TELEPORTERS_SIZE), SEEK_CUR);
  fseek(dfile, dungeonData->header.numTexts       * (DNGLIST_TEXTS_SIZE), SEEK_CUR);
  fseek(dfile, dungeonData->header.numActuators   * (DNGLIST_ACTUATORS_SIZE), SEEK_CUR);
  fseek(dfile, dungeonData->header.numCreatures   * (DNGLIST_CREATURES_SIZE), SEEK_CUR);
  fseek(dfile, dungeonData->header.numWeapons     * (DNGLIST_WEAPONS_SIZE), SEEK_CUR);
  fseek(dfile, dungeonData->header.numClothes     * (DNGLIST_CLOTHES_SIZE), SEEK_CUR);
  fseek(dfile, dungeonData->header.numScrolls     * (DNGLIST_SCROLLS_SIZE), SEEK_CUR);
  fseek(dfile, dungeonData->header.numPotions     * (DNGLIST_POTIONS_SIZE), SEEK_CUR);
  fseek(dfile, dungeonData->header.numChests      * (DNGLIST_CHESTS_SIZE), SEEK_CUR);
  fseek(dfile, dungeonData->header.numMiscItems   * (DNGLIST_MISCITEMS_SIZE), SEEK_CUR);
  fseek(dfile, dungeonData->header.numMissiles    * (DNGLIST_MISSILES_SIZE), SEEK_CUR);
  fseek(dfile, dungeonData->header.numClouds      * (DNGLIST_CLOUDS_SIZE), SEEK_CUR);
  
  /* read map tile data */
  dungeonData->mapData = (DngMapData **) malloc(sizeof(DngMapData *) * numMaps);

  for(i = 0; i < numMaps; i++){
    dungeonData->mapData[i] = readDngMapData(dfile, dungeonData->maps[i]);
    printf("map %d: # creatures = %d\n", i, mapGetNumCreatures(dungeonData->maps[i]));
  }

#if 0
  for(i = 0; i < 19; i++){
    printf("tile = 0x%x\n", dungeonData->mapData[0]->tiles[i]);
  }

  for(i = 0; i < 19; i++){
    printf("tile type = 0x%x\n", tileGetType(dungeonData->mapData[0]->tiles[i]));
  }

  for(i = 0; i < 19; i++){
    if(tileGetType(dungeonData->mapData[0]->tiles[i]) == 0) printf("*"); else printf("-");
  }
  printf("\n");
#endif

#if 0
  printf("map %d origin = (%d,%d)\n", 10, dungeonData->maps[10]->xoffset, dungeonData->maps[10]->yoffset);
  printDngMapTiles(dungeonData->mapData[10]);
  printf("map %d origin = (%d,%d)\n", 11, dungeonData->maps[11]->xoffset, dungeonData->maps[11]->yoffset);
  printDngMapTiles(dungeonData->mapData[11]);
  printf("map %d origin = (%d,%d)\n", 14, dungeonData->maps[14]->xoffset, dungeonData->maps[14]->yoffset);
  printDngMapTiles(dungeonData->mapData[14]);
  printf("map %d origin = (%d,%d)\n", 15, dungeonData->maps[15]->xoffset, dungeonData->maps[15]->yoffset);
  printDngMapTiles(dungeonData->mapData[15]);
  printf("map %d origin = (%d,%d)\n", 16, dungeonData->maps[16]->xoffset, dungeonData->maps[16]->yoffset);
  printDngMapTiles(dungeonData->mapData[16]);
  printf("map %d origin = (%d,%d)\n", 17, dungeonData->maps[17]->xoffset, dungeonData->maps[17]->yoffset);
  printDngMapTiles(dungeonData->mapData[17]);
  printf("map %d origin = (%d,%d)\n", 18, dungeonData->maps[18]->xoffset, dungeonData->maps[18]->yoffset);
  printDngMapTiles(dungeonData->mapData[18]);
  printf("map %d origin = (%d,%d)\n", 19, dungeonData->maps[19]->xoffset, dungeonData->maps[19]->yoffset);
  printDngMapTiles(dungeonData->mapData[19]);
  printf("map %d origin = (%d,%d)\n", 22, dungeonData->maps[22]->xoffset, dungeonData->maps[22]->yoffset);
  printDngMapTiles(dungeonData->mapData[22]);
  printf("map %d origin = (%d,%d)\n", 23, dungeonData->maps[23]->xoffset, dungeonData->maps[23]->yoffset);
  printDngMapTiles(dungeonData->mapData[23]);
  printf("map %d origin = (%d,%d)\n", 24, dungeonData->maps[24]->xoffset, dungeonData->maps[24]->yoffset);
  printDngMapTiles(dungeonData->mapData[24]);
  printf("map %d origin = (%d,%d)\n", 25, dungeonData->maps[25]->xoffset, dungeonData->maps[25]->yoffset);
  printDngMapTiles(dungeonData->mapData[25]);
  printf("map %d origin = (%d,%d)\n", 26, dungeonData->maps[26]->xoffset, dungeonData->maps[26]->yoffset);
  printDngMapTiles(dungeonData->mapData[26]);
  printf("map %d origin = (%d,%d)\n", 27, dungeonData->maps[27]->xoffset, dungeonData->maps[27]->yoffset);
  printDngMapTiles(dungeonData->mapData[27]);
  printf("map %d origin = (%d,%d)\n", 28, dungeonData->maps[28]->xoffset, dungeonData->maps[28]->yoffset);
  printDngMapTiles(dungeonData->mapData[28]);
  printf("map %d origin = (%d,%d)\n", 29, dungeonData->maps[29]->xoffset, dungeonData->maps[29]->yoffset);
  printDngMapTiles(dungeonData->mapData[29]);
  printf("map %d origin = (%d,%d)\n", 30, dungeonData->maps[30]->xoffset, dungeonData->maps[30]->yoffset);
  printDngMapTiles(dungeonData->mapData[30]);
  printf("map %d origin = (%d,%d)\n", 31, dungeonData->maps[31]->xoffset, dungeonData->maps[31]->yoffset);
  printDngMapTiles(dungeonData->mapData[31]);
  printf("map %d origin = (%d,%d)\n", 32, dungeonData->maps[32]->xoffset, dungeonData->maps[32]->yoffset);
  printDngMapTiles(dungeonData->mapData[32]);
  printf("map %d origin = (%d,%d)\n", 33, dungeonData->maps[33]->xoffset, dungeonData->maps[33]->yoffset);
  printDngMapTiles(dungeonData->mapData[33]);
  printf("map %d origin = (%d,%d)\n", 34, dungeonData->maps[34]->xoffset, dungeonData->maps[34]->yoffset);
  printDngMapTiles(dungeonData->mapData[34]);
  printf("map %d origin = (%d,%d)\n", 37, dungeonData->maps[37]->xoffset, dungeonData->maps[37]->yoffset);
  printDngMapTiles(dungeonData->mapData[37]);
  printf("map %d origin = (%d,%d)\n", 39, dungeonData->maps[39]->xoffset, dungeonData->maps[39]->yoffset);
  printDngMapTiles(dungeonData->mapData[39]);
  printf("map %d origin = (%d,%d)\n", 41, dungeonData->maps[41]->xoffset, dungeonData->maps[41]->yoffset);
  printDngMapTiles(dungeonData->mapData[41]);
  printf("map %d origin = (%d,%d)\n", 42, dungeonData->maps[42]->xoffset, dungeonData->maps[42]->yoffset);
  printDngMapTiles(dungeonData->mapData[42]);
  printf("map %d origin = (%d,%d)\n", 43, dungeonData->maps[43]->xoffset, dungeonData->maps[43]->yoffset);
  printDngMapTiles(dungeonData->mapData[43]);
#endif

  printf("map %d origin = (%d,%d)\n", 0, dungeonData->maps[0]->xoffset, dungeonData->maps[0]->yoffset);
  //printDngMapTiles(dungeonData->mapData[0]);

  printf("map %d origin = (%d,%d)\n", 2, dungeonData->maps[2]->xoffset, dungeonData->maps[2]->yoffset);
  //printDngMapTiles(dungeonData->mapData[2]);

  return dungeonData;
}

void loadDngMap(){

}
