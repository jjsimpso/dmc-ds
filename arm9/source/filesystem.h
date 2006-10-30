#include <nds.h>                        // ndslib
#include <gbfs.h>			// filesystem functions

#include "dmc.h"

/* */
typedef struct {
  const char *data;
  Uint32 pos;
} GBFS_FD;

#define FILE GBFS_FD
#define fread gbfsRead
#define fseek gbfsSeek
#define ftell gbfsTell

int gbfsRead(void *ptr, size_t size, size_t nmemb, FILE *stream);
int gbfsSeek(FILE *stream, Uint32 offset, int whence);
int gbfsTell(FILE *stream);
