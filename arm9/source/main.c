/*---------------------------------------------------------------------------------
 
---------------------------------------------------------------------------------*/

//#include <nds.h>			// include your ndslib
//#include <nds/arm9/image.h>		// needed to load pcx files
//#include <nds/arm9/trig_lut.h>
#include <stdio.h>
#include <stdlib.h>

#include "dmc.h"
#include "data.h"
#include "draw.h"

/* Global vars */
#include "defpal.h"                     // Default palette data

/*
DngDat *dungeonData;
GfxDat *gfxData;
int *gfxndx;
*/
Globals G;
GBFS_FILE const* gbfs_file;


int main()
{	
	
	
	// Turn on everything
	powerON(POWER_ALL);
	
	// IRQ basic setup
	irqInit();
	irqSet(IRQ_VBLANK, 0);
	irqEnable(IRQ_VBLANK);

	videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE);
        videoSetModeSub(MODE_5_2D | DISPLAY_BG1_ACTIVE);// | DISPLAY_BG3_ACTIVE);

	vramSetBankA(VRAM_A_MAIN_BG_0x6000000);
        vramSetBankC(VRAM_C_SUB_BG_0x6200000);

        //BG0_CR = BG_MAP_BASE(31);
	BG2_CR = BG_BMP8_512x256 | BG_WRAP_ON;
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

	// Map Game Cartridge memory to ARM9
	WAIT_CR &= ~0x80;

	/* Start searching from the beginning of cartridge memory */
	gbfs_file = find_first_gbfs_file((void*)0x08000000);

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

	/* Load level's dungeon graphics */
	//initD
	//loadLevelGfx(wallGfx, "dungfx.txt");

	drawTitle();

	while(1){
	  swiWaitForVBlank();
	  scanKeys();
	  
	  if(keysDown() & KEY_UP) {
	    printf("Up pressed\n");
	  }

	}

	return 0;
}
