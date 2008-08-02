/*---------------------------------------------------------------------------------
 
---------------------------------------------------------------------------------*/

//#include <nds.h>			// include your ndslib
//#include <nds/arm9/image.h>		// needed to load pcx files
//#include <nds/arm9/trig_lut.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dmc.h"
#include "draw.h"
#include "actions.h"

/* Global vars */
#include "defpal.h"                     // Default palette data

Globals G;
#ifdef USE_GBFS
GBFS_FILE const* gbfs_file;
#endif

void initGlobals(){
#ifdef USE_GBFS
  /* Start searching from the beginning of cartridge memory */
  gbfs_file = find_first_gbfs_file((void*)0x08000000);
#else
  /* Initialize libfat */
  if(!fatInitDefault()){
    fprintf(stderr, "Error initializing libfat\n");
    exit(1);
  }

  /* change working directory */
  //chdir("/home/jonathan/coding/gba-ds/dmc/data");
  chdir("fat:/dmc");
#endif

  /* Load dungeon.dat */
  G.dungeonData = readDngDat("dungeon.dat");
  if(G.dungeonData == NULL){
    fprintf(stderr, "Error loading dungeon.dat file %s\n", "dungeon.dat");
    exit(1);
  }

  /* Load graphics.dat */
  G.gfxData = readGfxDat("GRAPHICS.DAT");
  if(G.gfxData == NULL){
    fprintf(stderr, "Error loading graphics.dat file %s\n", "GRAPHICS.DAT");
    exit(1);    
  }
  
  /* Load graphics.ndx (really just returns the pointer to the ndx array) */
  G.gfxndx = readGfxNdx("graphics.ndx");
  if(G.gfxndx == NULL){
    fprintf(stderr, "Error loading graphics.ndx file %s\n", "graphics.ndx");
    exit(1);
  }
  
  /* Load DM2 palette */
  copyPal24(DefaultPalette, BG_PALETTE, 0, 256);
  
  /* Init graphics surface */
  G.DngView = newHWSurf(256, 192, 1, 256, (Uint8 *)BG_BMP_RAM(3));
  
  
  G.WallGfx = (WallGfx*)malloc(sizeof(WallGfx));
  loadLevelGfx(G.WallGfx, "dungfx.txt");
  
  G.curMap = 0;
  G.x = 4; //dngGetStartX(G.dungeonData);
  G.y = 15; //dngGetStartY(G.dungeonData);
  G.facing = dngGetStartFacing(G.dungeonData);

}

int handleKeyEvents(){
  scanKeys();
 
  if(keysDown() & KEY_LEFT){
    turnLeft();
    printf("Pressed Left\n");
  }
  else if(keysDown() & KEY_RIGHT){
    turnRight();
    printf("Pressed Right\n");
  }
  else if(keysDown() & KEY_UP){
    stepForward();
    printf("Pressed Up\n");
  }
  else if(keysDown() & KEY_DOWN){
    printf("Pressed Down\n");
  }
  
  return 0;
}

void mainLoop(){
  
  while(1){
    //drawTest();
    handleKeyEvents();
    drawView(G.x, G.y, G.facing, G.DngView);
  }
  
  return;
}

int main()
{	
	
	
	// Turn on everything
	powerON(POWER_ALL);
	
	// IRQ basic setup
	irqInit();
	irqSet(IRQ_VBLANK, 0);
	//irqEnable(IRQ_VBLANK); I think this is redundant

	videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE);
        videoSetModeSub(MODE_5_2D | DISPLAY_BG1_ACTIVE);// | DISPLAY_BG3_ACTIVE);

	vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
        vramSetBankC(VRAM_C_SUB_BG_0x06200000);

        //BG0_CR = BG_MAP_BASE(31);
	BG2_CR = BG_BMP8_256x256 | BG_BMP_BASE(0);
        SUB_BG1_CR = BG_MAP_BASE(31);
	//SUB_BG3_CR = BG_BMP8_256x256 | BG_WRAP_ON;
        
        BG_PALETTE_SUB[255] = RGB15(31,31,31);  //by default font will be rendered with color 255
        
        //consoleInit() is a lot more flexible but this gets you up and running quick
        consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(31), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);
	
	printf("Hello World\n");
	
	// Setup rotation background
	BG2_XDX = 1 << 8;
        BG2_XDY = 0;
        BG2_YDX = 0;
        BG2_YDY = 1 << 8;
	BG2_CY = 0;
	BG2_CX = 0;
	
#ifdef USE_GBFS
	// Map Game Cartridge memory to ARM9
	REG_EXMEMCNT &= ~0x80;
#endif
	
	initGlobals();
	
	printf("BG2_CR = 0x%x\n", BG2_CR);
	printf("DngView->pixels = 0x%x\n", (unsigned int)G.DngView->pixels);
	//drawTest();
	drawView(G.x, G.y, G.facing, G.DngView);
	printf("BG2_CR = 0x%x\n", BG2_CR);
	printf("DngView->pixels = 0x%x\n", (unsigned int)G.DngView->pixels);
	/*
	printf("sizeof Surface = %d\n", sizeof(Surface));
	printf("offset w = %d\n", ((int)&G.DngView->w) - ((int)G.DngView));
	printf("offset h = %d\n", ((int)&G.DngView->h) - ((int)G.DngView));
	printf("offset bpr = %d\n", ((int)&G.DngView->bpr) - ((int)G.DngView));
	printf("offset pitch = %d\n", ((int)&G.DngView->pitch) - ((int)G.DngView));
	printf("offset bpp = %d\n", ((int)&G.DngView->bpp) - ((int)G.DngView));
	printf("offset pixels = %d\n", ((int)&G.DngView->pixels) - ((int)G.DngView));
	*/

	mainLoop();

	return 0;
}
