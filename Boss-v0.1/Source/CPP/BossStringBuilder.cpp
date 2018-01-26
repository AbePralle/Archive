//=============================================================================
//  BossStringBuilder.cpp
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"

namespace Boss
{

StringBuilder::StringBuilder( Integer initial_capacity )
  : count(0)
{
  Integer internal_capacity = sizeof(internal_buffer) / sizeof(Character);
  if (initial_capacity <= internal_capacity)
  {
    capacity = internal_capacity;
    characters = internal_buffer;
  }
  else
  {
    capacity = initial_capacity;
    characters = new Character[ capacity ];
  }
}

StringBuilder::~StringBuilder()
{
  if (characters != internal_buffer) delete characters;
}

StringBuilder& StringBuilder::log()
{
  Integer n = count;
  for (Integer i=0; i<n; ++i)
  {
    int ch = characters[i];
    if (ch <= 0x7f)
    {
      //0..0x7f (0-127) is written normally
      putc( (char)ch, stdout );
    }
    else if (ch <= 0x7ff)
    {
      //0x80..0x7ff is encoded in two bytes.
      //0000 0xxx yyyy yyyy -> 110xxxyy 10yyyyyy

      //top_5    = (1100 0000 | ((ch>>6) & 1 1111))
      int top_5    = (0xc0      | ((ch>>6) & 0x1f));

      //bottom_6 = (1000 0000 | (ch & 11 1111))
      int bottom_6 = (0x80      | (ch & 0x3f));

      putc( (char)top_5, stdout );
      putc( (char)bottom_6, stdout );
    }
    else
    {
      //0x8000..0xffff is encoded in three bytes.
      //xxxx xxxx yyyy yyyy -> 1110xxxx 10xxxxyy 10yyyyyy

      //top_4    = (1110 0000 | ((ch>>12) & 1111))
      int top_4    = (0xe0      | ((ch>>12) & 0xf));

      //mid_6    = (1000 0000 | ((ch>>6)  & 11 1111))
      int mid_6    = (0x80      | ((ch>>6)  & 0x3f));

      //bottom_6 = (1000 0000 | (ch & 11 1111))
      int bottom_6 = (0x80 | (ch & 0x3f));

      putc( (char)top_4, stdout );
      putc( (char)mid_6, stdout );
      putc( (char)bottom_6, stdout );
    }
  }
  return *this;
}

StringBuilder& StringBuilder::print( Character value )
{
  reserve( 1 );
  characters[ count++ ] = value;
  return *this;
}

StringBuilder& StringBuilder::print( Character* values, Integer len )
{
  reserve( len );
  memcpy( characters+count, values, len*sizeof(Character) );
  count += len;
  return *this;
}

StringBuilder& StringBuilder::print( Integer value )
{
  char st[20];
  sprintf( st, "%d", value );
  return print( st );
}

StringBuilder& StringBuilder::print( Long value )
{
  char st[40];
  sprintf( st, "%lld", value );
  return print( st );
}

StringBuilder& StringBuilder::print( Object* value )
{
  if ( !value ) return print( "null" );
  value->type->print( value, *this );
  return *this;
}

StringBuilder& StringBuilder::print( Real value )
{
  char st[512];
  real_to_c_string( value, st, 512 );
  return print( st );
}

StringBuilder& StringBuilder::print( const char value )
{
  reserve( 1 );
  characters[ count++ ] = value;
  return *this;
}

StringBuilder& StringBuilder::print( const char* value )
{
  Integer len = strlen( value );
  reserve( len );
  for (Integer i=0; i<len; ++i)
  {
    characters[count+i] = value[i];
  }
  count += len;
  return *this;
}

StringBuilder& StringBuilder::println() { return print('\n'); }
StringBuilder& StringBuilder::println( Character value ) { return print(value).print('\n'); }
StringBuilder& StringBuilder::println( Character* values, Integer len ) { return print(values,len).print('\n'); }
StringBuilder& StringBuilder::println( Integer value ) { return print(value).print('\n'); }
StringBuilder& StringBuilder::println( Long value ) { return print(value).print('\n'); }
StringBuilder& StringBuilder::println( Object* value ) { return print(value).print('\n'); }
StringBuilder& StringBuilder::println( Real value ) { return print(value).print('\n'); }
StringBuilder& StringBuilder::println( const char value ) { return print(value).print('\n'); }
StringBuilder& StringBuilder::println( const char* value ) { return print(value).print('\n'); }

StringBuilder& StringBuilder::reserve( Integer additional )
{
  Integer required = count + additional;
  if (capacity >= required) return *this; 

  Integer doubled = capacity << 1;
  if (doubled > required) required = doubled;

  Character* new_characters = new Character[ required ];
  memcpy( new_characters, characters, count*sizeof(Character) );

  if (characters != internal_buffer) delete characters;
  characters = new_characters;
  capacity = required;

  return *this;
}

}; // namespace Boss
