#ifndef DUNGEON_H_DEFINED
#define DUNGEON_H_DEFINED

/* MASKS */
#define DNGHDR_STARTPOS_FACING_MASK   0x0C00
#define DNGHDR_STARTPOS_Y_MASK        0x03E0
#define DNGHDR_STARTPOS_X_MASK        0x001F

#define DNGMAP_SIZE_LEVEL_MASK   0x003F
#define DNGMAP_SIZE_WIDTH_MASK   0x07C0
#define DNGMAP_SIZE_HEIGHT_MASK  0xF800

#define DNGMAP_NUMGFX_WALLS_MASK     0x000F
#define DNGMAP_NUMGFX_RANDWALLS_MASK 0x00F0
#define DNGMAP_NUMGFX_PADS_MASK      0x0F00
#define DNGMAP_NUMGFX_RANDPADS_MASK  0xF000

#define DNGMAP_MISC_ORNATE_MASK    0x000F
#define DNGMAP_MISC_CREATURES_MASK 0x00F0
#define DNGMAP_MISC_UNKNOWN_MASK   0x0F00
#define DNGMAP_MISC_DEPTH_MASK     0xF000

#define DNGMAP_DOORGFX_UNKNOWN_MASK    0x000F
#define DNGMAP_DOORGFX_STYLE_MASK      0X00F0
#define DNGMAP_DOORGFX_DOOR0GFX_MASK   0X0F00
#define DNGMAP_DOORGFX_DOORTYPE_MASK   0XF000
 
#define DNGMAPTILE_TYPE_MASK     0xE0
#define DNGMAPTILE_OBJECTS_MASK  0x10
#define DNGMAPTILE_ATTRIBS_MASK  0x0F

#define DNGMAPTEXT_CHAR3         0x001F
#define DNGMAPTEXT_CHAR2         0x03E0
#define DNGMAPTEXT_CHAR1         0x7C00


/* VALUES */
#define DNGLIST_LAST_ITEM        0xFFFE
#define DNGLIST_DOORS_SIZE       4   /* sizes are in bytes */
#define DNGLIST_TELEPORTERS_SIZE 6
#define DNGLIST_TEXTS_SIZE       4
#define DNGLIST_ACTUATORS_SIZE   8
#define DNGLIST_CREATURES_SIZE   16
#define DNGLIST_WEAPONS_SIZE     4
#define DNGLIST_CLOTHES_SIZE     4
#define DNGLIST_SCROLLS_SIZE     4
#define DNGLIST_POTIONS_SIZE     4
#define DNGLIST_CHESTS_SIZE      8
#define DNGLIST_MISCITEMS_SIZE   4
#define DNGLIST_MISSILES_SIZE    8
#define DNGLIST_CLOUDS_SIZE      4

/* DNG TEXT */
#define DNG_TEXT_TERM            31


typedef struct Node {
  Uint16 nextID;
  Uint16 *data;
  struct Node *next;
} DngListNode;

typedef struct {  
  Uint16 dataSize;     /* data size in words(16 bit words of course) */
  Uint16 numElements;
  DngListNode *first;
} DngList;

typedef struct {
  Uint16 seed;                /* Random Graphics seed (controls the random graphics displayed on some walls and floors) */
  Uint16 mapDataSizeBytes;    /* Map data size in bytes */
  Uint8  numMaps;             /* Number of maps */
  Uint8  unknown1;            /*  */
  Uint16 textDataSizeWords;   /* Text data size in words */
  Uint16 startingPos;         /* Starting party position: 0-4:x , 5-9:y , 10-11:facig */
  Uint16 objListSizeWords;    /* Object list size in words */
  Uint16 numDoors;            /*  */
  Uint16 numTeleporters;      /*  */
  Uint16 numTexts;            /*  */
  Uint16 numActuators;        /*  */
  Uint16 numCreatures;        /*  */
  Uint16 numWeapons;          /*  */
  Uint16 numClothes;          /*  */
  Uint16 numScrolls;          /*  */
  Uint16 numPotions;          /*  */
  Uint16 numChests;           /*  */
  Uint16 numMiscItems;        /*  */
  Uint8  unknown2[6];         /*  */
  Uint16 numMissiles;         /*  */
  Uint16 numClouds;           /*  */
} DngHeader;

typedef struct {
  Uint16 offset;      /* Offset of Map data in map data */
  Uint8  unknown1[4]; /*  */
  Uint8  xoffset;     /* Map offset x */
  Uint8  yoffset;     /* Map offset y */
  Uint16 size;        /* 0-5:level #, 6-10:width-1 , 11-15:height-1 */
  Uint16 numGfx;      /* 0-3:wall , 4-7:rand wall, 8-11:pad, 12-15:rand pad */
  Uint16 misc;        /* 0-3:num ornate, 4-7:num creatures, 8-11:unknown, 12-15:depth */
  Uint16 doorGfx;     /* 0-3:unknown, 4-7:gfx style, 8-11:gfx for door type 0, 12-15:door type 1 */
} DngMap;

typedef struct {
  Uint8 *tiles;
  Uint8 creatureGfx[16];
  Uint8 wallGfx[16];
  Uint8 padGfx[16];
  Uint8 ornateGfx[16];
  //Uint8 xorg; /* stored here solely for convenience */
  //Uint8 yorg; /* stored here solely for convenience */
  Uint8 w;    /* stored here solely for convenience */
  Uint8 h;    /* stored here solely for convenience */
} DngMapData;

typedef struct {
  DngHeader header;
  DngMap **maps;
  Uint16 *tilesWithObjsIndex;
  Uint16 *objList;  /* List includes first object only(one per tile) */
  Uint8 **textData;
  
  DngMapData **mapData;
} DngDat;


/* accessor functions */
Uint8 dngGetStartLevel(DngDat *dng);
Uint8 dngGetStartX(DngDat *dng);
Uint8 dngGetStartY(DngDat *dng);
Uint8 dngGetStartFacing(DngDat *dng);

Uint8 tileGetType(Uint8 tile);
Uint8 tileGetAttribs(Uint8 tile);

/* setter functions */
Uint8 tileNew(Uint8 type);
Uint8 tileSetType(Uint8 tile, Uint8 type);

/* map navigation */
Uint8 lookupTileAhead(int x, int y, int facing, DngMapData *map);
int lookupTile(int num, int x, int y, int facing, DngMapData *map, Uint8 *tile);


/* file I/O functions */
DngDat *readDngDat(char *filename);
int readDngHeader(FILE *file, DngHeader *hdr);
DngMap *readDngMap(FILE *file);


#define TILEATTR_STAIRS_DIR(tile)      (tileGetAttribs(tile) & 0x8)
#define TILEATTR_STAIRS_ORIENT(tile)   (tileGetAttribs(tile) & 0x4)

#define TILEATTR_PIT_VISIBLE(tile)     (tileGetAttribs(tile) & 0x4)
#define TILEATTR_PIT_OPEN(tile)        (tileGetAttribs(tile) & 0x8)

#define TILEATTR_DOOR_ORIENT(tile)   (tileGetAttribs(tile) & 0x8)


#endif
