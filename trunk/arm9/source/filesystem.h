#ifndef FILESYSTEM_H_INCLUDED
#define FILESYSTEM_H_INCLUDED

/* */
typedef struct {
  const char *data;
  Uint32 pos;
} GBFS_FD;

#define FILE GBFS_FD
#define fopen gbfsOpen
#define fread gbfsRead
#define fseek gbfsSeek
#define ftell gbfsTell
#define fgets gbfsGets

GBFS_FD *gbfsOpen(const char *path, const char *mode);
int gbfsClose(GBFS_FD *fp);
int gbfsRead(void *ptr, size_t size, size_t nmemb, FILE *stream);
int gbfsSeek(FILE *stream, Uint32 offset, int whence);
int gbfsTell(FILE *stream);
char *gbfsGets(char *s, int size, FILE *stream);

#endif /* FILESYSTEM_H_INCLUDED */
