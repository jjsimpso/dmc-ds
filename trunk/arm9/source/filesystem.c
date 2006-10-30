#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filesystem.h"

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

FILE *gbfsOpen(char *filename, const char *mode){
  FILE *stream;

  stream = malloc(sizeof(GBFS_FD));
  stream->data = gbfs_get_obj(&data_gbfs, filename, NULL);
  if(stream->data == NULL){
    return NULL;
  }
  stream->pos = 0;

  return stream;
}
