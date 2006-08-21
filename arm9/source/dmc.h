/* Convert SDL types to libnds types */
typedef u8 Uint8;
typedef u16 Uint16;
typedef u32 Uint32;

/* */
typedef struct {
  const char *data;
  Uint32 pos;
} GBFS_FD;

#define FILE GBFS_FD
#define fread gbfsRead
#define fseek gbfsSeek
#define ftell gbfsTell


