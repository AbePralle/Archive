//=============================================================================
//  BossParseReader.cpp
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"

namespace Boss
{

ParseReader::ParseReader()
  : data(0), count(0), spaces_per_tab(2)
{
}

ParseReader::~ParseReader()
{
  if (data)
  {
    delete data;
    data = 0;
  }
}

void ParseReader::init( VM* vm, const char* filepath )
{
  FILE* fp = fopen( filepath, "rb" );
  if ( !fp )
  {
    error( "Unable to open file \"" ).print( filepath ).print( "\" for reading." );
    BOSS_THROW( vm, COMPILE_ERROR );
  }

  fseek( fp, 0, SEEK_END );
  Integer utf8_count = (Integer) ftell( fp );
  fseek( fp, 0, SEEK_SET );

  char* utf8_data = new char[ utf8_count ];

  fread( utf8_data, 1, utf8_count, fp );
  fclose( fp );

  init( vm, filepath, utf8_data, utf8_count );

  delete utf8_data;
}

void ParseReader::init( VM* vm, const char* filepath, const char* utf8_data, Integer utf8_count )
{
  this->vm = vm;
  this->filepath = vm->consolidate( filepath );
  count = UTF8::decoded_count( utf8_data, utf8_count );
  data = new Character[ count ];
  UTF8::decode( utf8_data, utf8_count, data, count );
  sanitize_characters();
}

void ParseReader::init( VM* vm, const char* filepath, Character* data, Integer count )
{
  this->vm = vm;
  this->filepath = vm->consolidate( filepath );
  this->data = new Character[ count ];
  memcpy( this->data, data, count * sizeof(Character) );
  sanitize_characters();
}

Logical ParseReader::consume( Character value )
{
  if ( !has_another() || peek() != value) return false;
  read();
  return true;
}

Logical ParseReader::consume_keyword( const char* keyword )
{
  Integer len = (Integer) strlen(keyword);
  if (position.index + len > count) return false;

  ParseReaderPosition original_pos = position;
  for (int i=len; --i >= 0; )
  {
    if (keyword[i] != data[position.index+i])
    {
      position = original_pos;
      return false;
    }
  }
  position.index += len;

  if ( !has_another() ) return true;

  // Make sure next character isn't a continuation of some other ID
  Character ch = data[position.index]; 
  if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_')
  {
    position = original_pos;
    return false;
  }
  
  return true;
}

Logical ParseReader::consume_whitespace()
{
  Logical result = false;
  while (has_another())
  {
    switch (peek())
    {
      case '\t':
      case ' ':
        read();
        result |= true;
        break;

      default:
        return result;
    }
  }
  return result;
}

StringBuilder& ParseReader::error( const char* message )
{
  vm->error_filepath = filepath;
  vm->error_line = position.line;
  return vm->error_message.print( message );
}

void ParseReader::sanitize_characters()
{
  // Converts characters < 32 or == 127 to spaces.  Leaves newlines and tabs.
  Integer n = count + 1;
  Character* cur = data - 1;
  while (--n)
  {
    Character ch = *(++cur);
    if (ch < 32)
    {
      if (ch != '\n' && ch != '\t') *cur = ' ';
    }
    else if (ch == 127)
    {
      *cur = ' ';
    }
  }
}

Character ParseReader::peek( Integer lookahead )
{
  Integer i = position.index + lookahead;
  if ((i = position.index + lookahead) >= count) return 0;
  return data[ i ];
}

Character ParseReader::read()
{
  Character result = data[ position.index++ ];
  switch (result)
  {
    case '\t':
      position.column += spaces_per_tab;
      return result;

    case '\n':
      ++position.line;
      position.column = 1;
      return result;

    default:
      ++position.column;
      return result;
  }
}

}; // namespace Boss
