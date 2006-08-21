/*---------------------------------------------------------------------------------
 
---------------------------------------------------------------------------------*/

#include <nds.h>			// include your ndslib
#include <nds/arm9/image.h>		// needed to load pcx files
#include <nds/arm9/trig_lut.h>
#include <stdio.h>
#include <stdlib.h>

#include <gbfs.h>			// filesystem functions

#include "dmc.h"
#include "dungeon.h"

extern const GBFS_FILE  data_gbfs;

DngDat *dungeonData;


int main()
{	
	
	
	// Turn on everything
	powerON(POWER_ALL);
	
	// IRQ basic setup
	irqInit();
	irqSet(IRQ_VBLANK, 0);
	irqEnable(IRQ_VBLANK);

#if 0
	// Setup the Main screen for 2D 
	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	vramSetBankA(VRAM_A_MAIN_BG);

	BG0_CR = BG_MAP_BASE(31);

	// Set the colour of the font to White.
	BG_PALETTE[255] = RGB15(31,31,31);
	
	consoleInitDefault((u16*)SCREEN_BASE_BLOCK(31), (u16*)CHAR_BASE_BLOCK(0), 16);
#else
	videoSetMode(MODE_FB0);
        videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);  //sub bg 0 will be used to print text
	vramSetBankA(VRAM_A_LCD);
        vramSetBankC(VRAM_C_SUB_BG);

        SUB_BG0_CR = BG_MAP_BASE(31);
        
        BG_PALETTE_SUB[255] = RGB15(31,31,31);  //by default font will be rendered with color 255
        
        //consoleInit() is a lot more flexible but this gets you up and running quick
        consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(31), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);
#endif
	printf("Hello World\n");

	/* Load dungeon.dat */
	dungeonData = readDngDat("dungeon.dat");
	if(dungeonData == NULL){
	  fprintf(stderr, "Error loading dungeon.dat file %s\n", "dungeon.dat");
	  exit(1);    
	}

	while(1){
	  swiWaitForVBlank();
	  scanKeys();
	  
	  if(keysDown() & KEY_UP) {
	    printf("Up pressed\n");
	  }

	}

	return 0;
}
