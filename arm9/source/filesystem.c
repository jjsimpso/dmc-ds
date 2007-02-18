#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dmc.h"

GBFS_FD *gbfsOpen(const char *path, const char *mode){
  GBFS_FD *stream;

  stream = malloc(sizeof(GBFS_FD));
  stream->data = gbfs_get_obj(gbfs_file, path, NULL);
  if(stream->data == NULL){
    return NULL;
  }
  stream->pos = 0;

  return stream;
}

int gbfsClose(GBFS_FD *fp){
  if(fp != NULL){
    if(fp->data != NULL)
      free(fp->data);  

    free(fp);
  }
  return 0;
}

int gbfsRead(void *ptr, size_t size, size_t nmemb, FILE *stream){
  int i;

  for(i = 0; i < nmemb; i++){
    memcpy(ptr + (i * size), stream->data + stream->pos, size);
    stream->pos += size;
  }

  return size * nmemb;
}

int gbfsSeek(FILE *stream, Uint32 offset, int whence){

  if(whence == SEEK_SET){
    stream->pos = offset;
  }
  else if(whence == SEEK_CUR){
    stream->pos += offset;
  }
  else if(whence == SEEK_END){
    stream->pos = sizeof(*stream->data) - offset;
  }

  return 0;
}

int gbfsTell(FILE *stream){
  return stream->pos;
}

char *gbfsGets(char *s, int size, FILE *stream){
  strncpy(s, stream->data, size);
  s[255] = 0;
  return s;
}
