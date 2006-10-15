typedef struct {
  Uint16 numItems;
  Uint32 size1;
  Uint16 *size;
#ifdef ARM9
  
#else
  FILE *gfile;
#endif
} GfxDat;

typedef struct {
  Uint16 w, h;
  Uint8 pal[6];
  Uint8 *pixels;
} C4Img;

typedef struct {
  Uint16 w, h;
  Uint8 xoff, yoff;
  Uint8 ctype;
  Uint8 *pixels;
} C8Img;

Uint8 DM2FourBitPalMap[16];
/*
Uint8 FourBitPalMap[16] = {
  234,
  13,
  235,
  12,
  73,
  237,
  11,
  233,
  10,
  71,
  9,
  8,
  231,
  7,
  6,
  97
};
*/

Uint8 DefaultPalette[256][3] = {
  0x00,0x00,0x00,
  0x0C,0x08,0x00,
  0x1C,0x10,0x00,
  0x28,0x18,0x00,
  0x38,0x20,0x04,
  0x45,0x28,0x04,
  0x55,0x30,0x04,
  0x61,0x38,0x08,
  0x75,0x49,0x10,
  0x86,0x59,0x18,
  0x9A,0x65,0x20,
  0xAA,0x75,0x28,
  0xBA,0x86,0x30,
  0xCB,0x96,0x38,
  0xDB,0xA6,0x41,
  0xEF,0xB6,0x49,
  0x00,0x00,0x00,
  0x08,0x04,0x0C,
  0x14,0x08,0x18,
  0x1C,0x0C,0x24,
  0x28,0x10,0x30,
  0x34,0x14,0x3C,
  0x3C,0x18,0x49,
  0x49,0x20,0x59,
  0x59,0x24,0x6D,
  0x6D,0x2C,0x82,
  0x7D,0x30,0x96,
  0x92,0x38,0xAA,
  0xA2,0x41,0xC3,
  0xB6,0x45,0xD7,
  0xC7,0x4D,0xEB,
  0xDB,0x55,0xFF,
  0x00,0x00,0x00,
  0x00,0x00,0x0C,
  0x00,0x00,0x18,
  0x00,0x00,0x24,
  0x00,0x00,0x30,
  0x00,0x00,0x3C,
  0x00,0x00,0x49,
  0x00,0x00,0x55,
  0x00,0x00,0x69,
  0x00,0x00,0x7D,
  0x00,0x00,0x96,
  0x00,0x00,0xAA,
  0x00,0x00,0xBE,
  0x00,0x00,0xD7,
  0x00,0x00,0xEB,
  0x00,0x00,0xFF,
  0x00,0x00,0x00,
  0x04,0x04,0x10,
  0x08,0x0C,0x20,
  0x10,0x14,0x30,
  0x14,0x1C,0x41,
  0x1C,0x24,0x51,
  0x20,0x2C,0x65,
  0x28,0x34,0x75,
  0x24,0x4D,0x86,
  0x1C,0x69,0x96,
  0x18,0x82,0xAA,
  0x14,0x9A,0xBA,
  0x10,0xB2,0xCB,
  0x08,0xCF,0xDF,
  0x04,0xE7,0xEF,
  0x00,0xFF,0xFF,
  0x00,0x00,0x00,
  0x10,0x0C,0x08,
  0x20,0x1C,0x14,
  0x34,0x2C,0x1C,
  0x45,0x3C,0x28,
  0x59,0x4D,0x34,
  0x69,0x5D,0x3C,
  0x79,0x6D,0x49,
  0x8E,0x7D,0x55,
  0x9E,0x8E,0x5D,
  0xB6,0xAE,0x6D,
  0xCF,0xCB,0x79,
  0xDB,0xD7,0x8E,
  0xE7,0xDF,0x9E,
  0xF3,0xEB,0xB2,
  0xFF,0xF3,0xC7,
  0x00,0x00,0x00,
  0x08,0x0C,0x08,
  0x10,0x1C,0x10,
  0x18,0x2C,0x18,
  0x20,0x3C,0x20,
  0x28,0x49,0x28,
  0x30,0x59,0x30,
  0x38,0x69,0x38,
  0x45,0x79,0x45,
  0x51,0x8E,0x51,
  0x61,0x9E,0x61,
  0x71,0xB2,0x71,
  0x82,0xC7,0x82,
  0x92,0xDB,0x92,
  0x9E,0xEB,0x9E,
  0xAE,0xFF,0xAE,
  0x00,0x00,0x00,
  0x04,0x08,0x04,
  0x08,0x14,0x08,
  0x0C,0x20,0x0C,
  0x10,0x2C,0x10,
  0x14,0x38,0x14,
  0x20,0x51,0x20,
  0x14,0x61,0x20,
  0x08,0x71,0x20,
  0x00,0x82,0x20,
  0x04,0x8A,0x38,
  0x08,0x96,0x55,
  0x10,0xA2,0x71,
  0x2C,0xB6,0x7D,
  0x49,0xCB,0x8A,
  0x65,0xDF,0x9A,
  0x00,0x00,0x00,
  0x00,0x08,0x00,
  0x00,0x10,0x00,
  0x00,0x18,0x00,
  0x00,0x24,0x00,
  0x00,0x2C,0x00,
  0x00,0x34,0x00,
  0x00,0x49,0x00,
  0x00,0x59,0x00,
  0x00,0x6D,0x00,
  0x00,0x7D,0x00,
  0x00,0x92,0x00,
  0x00,0xB2,0x00,
  0x00,0xD3,0x00,
  0x00,0xE7,0x00,
  0x00,0xFF,0x00,
  0x00,0x00,0x00,
  0x10,0x10,0x00,
  0x20,0x20,0x00,
  0x30,0x30,0x00,
  0x45,0x45,0x00,
  0x55,0x55,0x00,
  0x65,0x65,0x00,
  0x75,0x75,0x00,
  0x8A,0x8A,0x00,
  0x9A,0x9A,0x00,
  0xAA,0xAA,0x00,
  0xBA,0xBA,0x00,
  0xCF,0xCF,0x00,
  0xDF,0xDF,0x00,
  0xEF,0xEF,0x00,
  0xFF,0xFF,0x00,
  0x00,0x00,0x00,
  0x10,0x0C,0x00,
  0x20,0x18,0x00,
  0x30,0x24,0x00,
  0x45,0x30,0x00,
  0x55,0x3C,0x00,
  0x65,0x49,0x00,
  0x75,0x55,0x00,
  0x8A,0x61,0x00,
  0x9A,0x71,0x00,
  0xAA,0x7D,0x00,
  0xBA,0x8A,0x00,
  0xCF,0x96,0x00,
  0xDF,0xA2,0x00,
  0xEF,0xAE,0x00,
  0xFF,0xBA,0x00,
  0x00,0x00,0x00,
  0x10,0x08,0x00,
  0x20,0x10,0x00,
  0x30,0x18,0x00,
  0x45,0x24,0x00,
  0x55,0x2C,0x00,
  0x65,0x34,0x00,
  0x75,0x3C,0x00,
  0x8A,0x49,0x00,
  0x9A,0x51,0x00,
  0xAA,0x59,0x00,
  0xBA,0x61,0x00,
  0xCF,0x6D,0x00,
  0xDF,0x75,0x00,
  0xEF,0x7D,0x00,
  0xFF,0x8A,0x00,
  0x00,0x00,0x00,
  0x10,0x00,0x00,
  0x20,0x00,0x00,
  0x30,0x00,0x00,
  0x45,0x00,0x00,
  0x55,0x00,0x00,
  0x65,0x00,0x00,
  0x75,0x00,0x00,
  0x8A,0x00,0x00,
  0x9A,0x00,0x00,
  0xAA,0x00,0x00,
  0xBA,0x00,0x00,
  0xCF,0x00,0x00,
  0xDF,0x00,0x00,
  0xEF,0x00,0x00,
  0xFF,0x00,0x00,
  0x00,0x00,0x00,
  0x04,0x00,0x00,
  0x0C,0x04,0x00,
  0x10,0x08,0x00,
  0x18,0x0C,0x00,
  0x20,0x0C,0x00,
  0x2C,0x14,0x04,
  0x34,0x18,0x04,
  0x41,0x1C,0x04,
  0x4D,0x20,0x08,
  0x59,0x28,0x08,
  0x65,0x30,0x10,
  0x6D,0x38,0x14,
  0x79,0x45,0x1C,
  0x86,0x4D,0x20,
  0x92,0x55,0x28,
  0x00,0x00,0x00,
  0x10,0x0C,0x08,
  0x20,0x18,0x10,
  0x34,0x24,0x1C,
  0x45,0x30,0x24,
  0x55,0x3C,0x2C,
  0x69,0x49,0x38,
  0x79,0x55,0x41,
  0x8A,0x61,0x49,
  0x9E,0x6D,0x55,
  0xAE,0x79,0x5D,
  0xBE,0x86,0x65,
  0xD3,0x92,0x71,
  0xDF,0xA6,0x7D,
  0xEF,0xBA,0x8A,
  0xFF,0xCF,0x9A,
  0x00,0x00,0x00,
  0x08,0x08,0x08,
  0x14,0x14,0x10,
  0x20,0x20,0x18,
  0x2C,0x2C,0x24,
  0x38,0x38,0x2C,
  0x45,0x45,0x34,
  0x51,0x51,0x41,
  0x61,0x61,0x51,
  0x71,0x71,0x61,
  0x82,0x82,0x71,
  0x92,0x92,0x82,
  0xA2,0xA2,0x92,
  0xB2,0xB2,0xA2,
  0xC3,0xC3,0xB2,
  0xD3,0xD3,0xC3,
  0x00,0x00,0x00,
  0x08,0x08,0x08,
  0x14,0x14,0x14,
  0x1C,0x1C,0x1C,
  0x28,0x28,0x28,
  0x34,0x34,0x34,
  0x3C,0x3C,0x3C,
  0x51,0x51,0x51,
  0x61,0x61,0x61,
  0x71,0x71,0x71,
  0x92,0x92,0x92,
  0xA2,0xA2,0xA2,
  0xB2,0xB2,0xB2,
  0xCB,0xCB,0xCB,
  0xE7,0xE7,0xE7,
  0xFF,0xFF,0xFF
};
