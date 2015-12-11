#ifndef BARD_BC_READER
#define BARD_BC_READER

#include "Bard.h"

typedef struct BardReader
{
  unsigned char* data;
  int            count;
  int            position;
  int            free_data_on_close;
} BardReader;

BardReader* BardReader_create_with_filename( const char* filename );
BardReader* BardReader_create_with_data( const char* filename, char* data, int count, int free_data_on_close );
void        BardReader_free( BardReader* reader );

BardReader* BardReader_init_with_filename( BardReader* reader, const char* filename );
BardReader* BardReader_init_with_data( BardReader* reader, const char* filename, char* data, int count, int free_data_on_close );

BardReader* BardReader_close( BardReader* reader );

int         BardReader_read_byte( BardReader* reader );
int         BardReader_read_integer( BardReader* reader );
int         BardReader_read_integer( BardReader* reader );
int         BardReader_consume( BardReader* reader, const char* text );
char*       BardReader_read_id( BardReader* reader );

#endif // BARD_BC_READER
