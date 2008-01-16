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
    /* Shouldn't do this, data is in ROM
    if(fp->data != NULL)
      free(fp->data);  
    */
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

// Still doesn't handle EOF
char *gbfsGets(char *s, int size, FILE *stream){
  const char *cursor;
  int i;

  cursor = stream->data + stream->pos;

  for(i = 0; i < size-1; i++, cursor++){
    if(*cursor == '\n'){
      s[i++] = '\n';
      s[i] = 0;
      stream->pos += i;
      return s;
    }
    else {
      s[i] = *cursor;
    }
  }

  s[i] = 0;
  stream->pos += i;

  return s;
}
