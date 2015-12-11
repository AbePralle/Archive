#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "Bard.h"

BardReader* BardReader_create_with_filename( const char* filename )
{
  return BardReader_init_with_filename( (BardReader*) BARD_ALLOCATE(sizeof(BardReader)), filename );
}

BardReader* BardReader_create_with_data( const char* filename, char* data, int count, int free_data_on_close )
{
  return BardReader_init_with_data( (BardReader*) BARD_ALLOCATE(sizeof(BardReader)), filename, data, count, free_data_on_close );
}

void BardReader_free( BardReader* reader )
{
  BARD_FREE( BardReader_close(reader) );
}


BardReader* BardReader_init_with_filename( BardReader *reader, const char* filename )
{
  FILE* fp = fopen( filename, "rb" );
  if ( !fp ) return NULL;

  fseek( fp, 0, SEEK_END );
  int size = (int) ftell( fp );
  fseek( fp, 0, SEEK_SET );
  if (size <= 0) return NULL;

  char* buffer = BARD_ALLOCATE( size );
  fread( buffer, 1, size, fp );

  fclose( fp );

  return BardReader_init_with_data( reader, filename, buffer, size, 1 );
}

BardReader* BardReader_init_with_data( BardReader *reader, const char* filename, char* data, int count, int free_data_on_close )
{
  bzero( reader, sizeof(BardReader) );
  reader->data = (unsigned char*) data;
  reader->count = count;
  reader->free_data_on_close = free_data_on_close;
  return reader;
}

BardReader* BardReader_close( BardReader *reader )
{
  if (reader->free_data_on_close)
  {
    BARD_FREE( reader->data );
    bzero( reader, sizeof(BardReader) );  // clear 'data' and 'free_data_on_close'
  }

  return reader;
}

int BardReader_read_byte( BardReader* reader )
{
  if (reader->position >= reader->count) return -1;
  return reader->data[ reader->position++ ];
}

int BardReader_read_integer( BardReader* reader )
{
  // 0: 0xxxxxxx - 0..127
  // 1: 1000xxxx - 1 byte follows (12 bits total, unsigned)
  // 2: 1001xxxx - 2 bytes follow (20 bits total, unsigned)
  // 3: 1010xxxx - 3 bytes follow (28 bits total, unsigned)
  // 4: 10110000 - 4 bytes follow (32 bits total, signed)
  // 5: 11xxxxxx - -64..-1
  int b1;
  
  b1 = BardReader_read_byte( reader );

  if (b1 < 128)  return b1;          // encoding 0
  if (b1 >= 192) return (b1 - 256);  // encoding 5

  switch (b1 & 0x30)
  {
    case 0:  // encoding 1
      return ((b1 & 15) << 8) | BardReader_read_byte(reader);

    case 16:  // encoding 2
      {
        int b2 = BardReader_read_byte(reader);
        int b3 = BardReader_read_byte(reader);
        return ((b1 & 15) << 16) | (b2 << 8) | b3;
      }

    case 32:  // encoding 3
      {
        int b2 = BardReader_read_byte(reader);
        int b3 = BardReader_read_byte(reader);
        int b4 = BardReader_read_byte(reader);
        return ((b1 & 15) << 24) | (b2 << 16) | (b3 << 8) | b4;
      }

    case 48:  // encoding 4
      {
        int result = BardReader_read_byte( reader );
        result = (result << 8) | BardReader_read_byte(reader);
        result = (result << 8) | BardReader_read_byte(reader);
        result = (result << 8) | BardReader_read_byte(reader);
        return (BardInteger) result;
      }

    default:
      return -1;
  }
}

int BardReader_consume( BardReader *reader, const char* text )
{
  int ch, i;
  int original_position = reader->position;
  for (i=0; (ch=text[i]); ++i)
  {
    if (BardReader_read_byte(reader) != ch)
    {
      reader->position = original_position;
      return 0;
    }
  }
  return 1;
}

char* BardReader_read_id( BardReader* reader )
{
  int count = BardReader_read_integer( reader );
  char* buffer = BARD_ALLOCATE( count+1 );

  {
    int i = count;
    for (i=0; i<count; ++i)
    {
      buffer[i] = (char) BardReader_read_integer( reader );
    }
  }

  buffer[count] = 0;
  return buffer;
}

