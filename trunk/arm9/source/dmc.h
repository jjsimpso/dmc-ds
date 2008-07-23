#ifndef DMC_H_INCLUDED
#define DMC_H_INCLUDED

/* Common includes */
#include <nds.h>			// ndslib
#ifdef USE_GBFS
#include <gbfs.h>			// filesystem functions
#else
#include <fat.h>
#endif

/* Convert SDL types to libnds types */
typedef u8 Uint8;
typedef u16 Uint16;
typedef u32 Uint32;
typedef s8 Sint8;
typedef s16 Sint16;
typedef s32 Sint32;

#include "filesystem.h"
#include "dungeon.h"
#include "graphics.h"
#include "surface.h"
#include "data.h"
#include "globals.h"

/* debug options */
#define DMC_DEBUG
#define DEBUG_LEVEL 1
#ifdef DMC_DEBUG
#define DEBUGF(level, args) \
    do { if (DEBUG_LEVEL >= (level)) {printf args;} } while (0)
#else
#define DEBUGF(level, args)     /* nothing */
#endif


#endif /* DMC_H_INCLUDED */
