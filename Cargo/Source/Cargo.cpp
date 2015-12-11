//=============================================================================
//  Cargo.cpp
//
//  A C++ JSON-compatible data modeling library.
//
//  https://github.com/Plasmaworks/Cargo
//  By Abe.Pralle (at) plasmaworks.com
//
//  ---------------------------------------------------------------------------
//
//  LICENSE
//
//  This is free and unencumbered software released into the public domain.
//  
//  Anyone is free to copy, modify, publish, use, compile, sell, or
//  distribute this software, either in source code form or as a compiled
//  binary, for any purpose, commercial or non-commercial, and by any
//  means.
//  
//  In jurisdictions that recognize copyright laws, the author or authors
//  of this software dedicate any and all copyright interest in the
//  software to the public domain. We make this dedication for the benefit
//  of the public at large and to the detriment of our heirs and
//  successors. We intend this dedication to be an overt act of
//  relinquishment in perpetuity of all present and future rights to this
//  software under copyright law.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
//  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//  OTHER DEALINGS IN THE SOFTWARE.
//  
//  For more information, please refer to <http://unlicense.org>
// 
//=============================================================================
#include "Cargo.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fcntl.h>

#if defined(_WIN32)
#  include <windows.h>
#else
#  include <sys/syslimits.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#endif

using namespace std;

namespace Cargo
{

// GLOBALS
Value        PI( acos(-1) );

NullData     null_singleton;
Value        Null( &null_singleton );

True         true_singleton;
False        false_singleton;
GlobalString empty_string_singleton( "" );
GlobalString null_string_singleton( "null" );
GlobalString false_string_singleton( "false" );
GlobalString true_string_singleton( "true" );

//=============================================================================
//  Value
//=============================================================================
Value::Value()
{
  data = null_singleton.retain();
}

Value::Value( const Value& other )
{
  data = other.data->retain();
}

Value::Value( const Value* other )
{
  data = other->data->retain();
}

Value::Value( Data* data )
{
  this->data = data->retain();
}

Value::Value( char value )
{
  char st[2];
  st[0] = value;
  st[1] = 0;
  data = (new String(st))->retain();
}

Value::Value( const char* value )
{
  if ( !value )
  {
    data = null_singleton.retain();
  }
  if (value[0] == 0)
  {
    data = empty_string_singleton.retain();
  }
  else
  {
    data = (new String(value))->retain();
  }
}

Value::Value( Character value )
{
  Character st[1];
  st[0] = value;
  data = (new String(st,1))->retain();
}

Value::Value( Character* value, Integer character_count )
{
  if ( !value )
  {
    data = null_singleton.retain();
  }
  if (character_count == 0)
  {
    data = empty_string_singleton.retain();
  }
  else
  {
    data = (new String(value,character_count))->retain();
  }
}

Value::Value( Logical value )
{
  if (value) data = true_singleton.retain();
  else       data = false_singleton.retain();
}

Value::Value( Integer value )
{
  data = (new Number(value))->retain();
}

Value::Value( Long value )
{
  data = (new Number((Real)value))->retain();
}

Value::Value( Real value )
{
  data = (new Number(value))->retain();
}

Value::Value( void* value, NativeDataReleaseFn release_fn )
{
  data = (new NativeData(value,release_fn))->retain();
}

Value::~Value()
{
  data = data->release();
}

Value&  Value::add( Value value )
{
  data->add( value.data ); return *this;
}

Value& Value::add_all( Value list )
{
  Integer n = list.count();
  for (Integer i=0; i<n; ++i)
  {
    add( list.get(i) );
  }
  return *this;
}

Value Value::at( Integer index ) const
{
  return data->at( index );
}

Character Value::character_at( Integer index ) const
{
  return data->character_at( index );
}

Character* Value::characters() const
{
  return data->characters();
}

Value& Value::clear()
{
  data->clear();
  return *this;
}

Value& Value::clone()
{
  Data* cloned_data = data->clone()->retain();
  data->release();
  data = cloned_data;
  return *this;
}

Logical Value::contains( Value value ) const
{
  return data->contains( value );
}

Integer Value::count() const
{
  return data->count();
}

Value Value::from( Integer i1 )
{
  return from( i1, count()-1 );
}

Value Value::from( Integer i1, Integer i2 )
{
  return Value( data->from(i1,i2) );
}

Value   Value::get( Integer index ) const { return Value(data->get(index)); }
Value   Value::get( Value key ) const { return Value(data->get(key)); }

Integer Value::get_integer( Value key, Integer default_value ) const
{
  TableEntry* entry = data->find( key );
  if ( !entry ) return default_value;

  return entry->value.to_integer();
}

Value   Value::get_list( Value key ) const
{
  TableEntry* entry = data->find( key );
  if ( !entry || !entry->value.is_list() ) return List();

  return entry->value;
}

Logical Value::get_logical( Value key, Logical default_value ) const
{
  TableEntry* entry = data->find( key );
  if ( !entry ) return default_value;

  return entry->value.to_logical();
}

Real Value::get_real( Value key, Real default_value ) const
{
  TableEntry* entry = data->find( key );
  if ( !entry ) return default_value;

  return entry->value.to_real();
}

Value   Value::get_string( Value key, Value default_value ) const
{
  TableEntry* entry = data->find( key );
  if ( !entry ) return default_value;

  return entry->value.to_string();
}

Value   Value::get_table( Value key ) const
{
  TableEntry* entry = data->find( key );
  if ( !entry || !entry->value.is_table() ) return Table();

  return entry->value;
}


Integer Value::hash_code() { return data->hash_code(); }
Value&  Value::insert( Value value ) { data->insert( value.data ); return *this; }

Logical Value::is_null()    const { return data->type() == CARGO_NULL; }
Logical Value::is_logical() const { Type t = data->type(); return t == CARGO_FALSE || t == CARGO_TRUE; }
Logical Value::is_integer() const { return data->type() == CARGO_NUMBER && to_integer() == to_real(); }
Logical Value::is_number()  const { return data->type() == CARGO_NUMBER; }
Logical Value::is_string()  const { return data->type() == CARGO_STRING; }
Logical Value::is_list()    const { return data->type() == CARGO_LIST; }
Logical Value::is_table()   const { return data->type() == CARGO_TABLE; }
Logical Value::is_native_data() const { return data->type() == CARGO_NATIVE_DATA; }

Integer Value::locate( Value value, Integer i1 ) const
{
  return data->locate( value, i1 );
}

Integer Value::locate_last( Value value ) const
{
  return data->locate_last( value, count() - 1 );
}

Integer Value::locate_last( Value value, Integer i1 ) const
{
  return data->locate_last( value, i1 );
}

void* Value::native_data() const
{
  return data->native_data();
}

Value Value::operator+( const Value& other ) const
{
  Type this_type = data->type();
  Type other_type = other.data->type();

  switch (this_type)
  {
    case CARGO_FALSE:
      switch (other_type)
      {
        case CARGO_FALSE:  return *this;
        case CARGO_TRUE:   return other;
        case CARGO_STRING: return to_string() + other;
        case CARGO_LIST:   return List( 1 + other.count() ).add( this ).add_all( other );
        default:           return Value();  // null
      }

    case CARGO_TRUE:
      switch (other_type)
      {
        case CARGO_FALSE:  return *this;
        case CARGO_TRUE:   return *this;
        case CARGO_STRING: return to_string() + other;
        case CARGO_LIST:   return List( 1 + other.count() ).add( this ).add_all( other );
        default:           return Value();  // null
      }

    case CARGO_NUMBER:
      switch (other_type)
      {
        case CARGO_NUMBER: return Value( to_real() + other.to_real() );
        case CARGO_STRING: return to_string() + other;
        case CARGO_LIST:   return List( 1 + other.count() ).add( this ).add_all( other );
        default:           return Value();  // null
      }

    case CARGO_STRING:
    {
      Value other_string = other.to_string();

      Integer this_count = count();
      Integer other_count = other_string.count();

      if (this_count == 0)
      {
        return other_string;
      }
      else if (other_count == 0)
      {
        return *this;
      }
      else
      {
        String* result = new String( this_count + other_count );
        memcpy( result->character_data, ((String*)data)->character_data, this_count*sizeof(Character) );
        memcpy( result->character_data+this_count, ((String*)(other_string.data))->character_data, other_count*sizeof(Character) );
        return Value( result );
      }
    }

    case CARGO_LIST:
    {
      if (other_type == CARGO_LIST)
      {
        return List( count() + other.count() ).add_all( this ).add_all( other );
      }
      else
      {
        return List( count() + 1 ).add_all( this ).add( other );
      }
    }

    default:
      return Value();  // null
  }
}

Value Value::operator-( const Value& other ) const
{
  Type other_type = other.data->type();

  switch (data->type())
  {
    case CARGO_NUMBER:
      if (other_type == CARGO_NUMBER)
      {
        return Value( to_real() - other.to_real() );
      }
      else
      {
        return Value();
      }

    default:
      return Value();
  }
}

Value Value::operator*( const Value& other ) const
{
  Type this_type = data->type();
  Type other_type = other.data->type();

  switch (this_type)
  {
    case CARGO_NUMBER:
      if (other_type == CARGO_NUMBER)
      {
        return Value( to_real() * other.to_real() );
      }
      else if (other_type == CARGO_STRING)
      {
        return other * (*this);
      }
      else
      {
        return Value();
      }

    case CARGO_STRING:
      if (other_type == CARGO_NUMBER)
      {
        // lhs string repeated rhs number of times
        Integer n = other.to_integer();
        if (n <= 0) return Value( &empty_string_singleton );
        if (n == 1) return *this;

        Integer length = count();
        String* result = new String( length * n );
        Character* dest = result->character_data;
        Character* src  = ((String*)data)->character_data;
        for (int i=0; i<n; ++i)
        {
          memcpy( dest, src, length*sizeof(Character) );
          dest += length;
        }

        return Value( result );
      }
      else
      {
        return Value(); // null
      }

    default:
      return Value(); // null
  }
}

Value Value::operator/( const Value& other ) const
{
  Type other_type = other.data->type();

  switch (data->type())
  {
    case CARGO_NUMBER:
      if (other_type == CARGO_NUMBER)
      {
        return Value( to_real() / other.to_real() );
      }
      else
      {
        return Value();
      }

    default:
      return Value();
  }
}

Value Value::operator%( const Value& other ) const
{
  Type other_type = other.data->type();

  switch (data->type())
  {
    case CARGO_NUMBER:
      if (other_type == CARGO_NUMBER)
      {
        Real a = to_real();
        Real b = other.to_real();
        return Value( a - floor(a/b)*b );
      }
      else
      {
        return Value();
      }

    default:
      return Value();
  }
}

Value& Value::operator+=( const Value& other )
{
  if (is_list())
  {
    if (other.is_list()) return add_all( other );
    else                 return add( other );
  }
  else
  {
    return this->operator=( operator+(other) );
  }
}

Value& Value::operator-=( const Value& other )
{
  return operator=( operator-(other) );
}

Value& Value::operator*=( const Value& other )
{
  return operator=( operator*(other) );
}

Value& Value::operator/=( const Value& other )
{
  return operator=( operator/(other) );
}

Value& Value::operator%=( const Value& other )
{
  return operator=( operator%(other) );
}

Value& Value::operator=( const Value& other )
{
  other.data->retain();
  data->release();
  data = other.data;
  return *this;
}

Logical Value::operator==( const Value& other ) const
{
  return data->compare_to( other.data ) == 0;
}

Logical Value::operator!=( const Value& other ) const
{
  return data->compare_to( other.data ) != 0;
}

Logical Value::operator<=( const Value& other ) const
{
  return data->compare_to( other.data ) <= 0;
}

Logical Value::operator>=( const Value& other ) const
{
  return data->compare_to( other.data ) >= 0;
}

Logical Value::operator<( const Value& other ) const
{
  return data->compare_to( other.data ) < 0;
}

Logical Value::operator>( const Value& other ) const
{
  return data->compare_to( other.data ) > 0;
}

void Value::print() const
{
  StringBuilder buffer;
  Printer writer( &buffer );
  writer.print( *this );
  buffer.display();
}

void Value::println() const
{
  print();
  printf( "\n" );
}

Value Value::remove( Value key )
{
  return data->remove( key );
}

Value Value::remove_at( Integer index )
{
  return data->remove_at(index);
}

Value& Value::set( Value key, Value value )
{
  data->set( key, value );
  return *this;
}

char* Value::to_c_string( char* buffer, Integer buffer_size ) const
{
  Value st = this->to_string();

  if ( !buffer || buffer_size <= 0 )
  {
    buffer_size = st.count() + 1;
    buffer = new char[ buffer_size ];
  }

  Integer copy_count = count();
  if (copy_count+1 > buffer_size) copy_count = buffer_size - 1;

  Character* characters = ((String*)(st.data))->character_data;
  for (int i=0; i<copy_count; ++i)
  {
    buffer[i] = (char) characters[i];
  }

  buffer[copy_count] = 0;
  return buffer;
}

Integer Value::to_integer() const { return data->to_integer(); }
Value   Value::to_json() const { return Value(data->to_json()); }
Logical Value::to_logical() const { return data->to_logical(); }
Real    Value::to_real() const { return data->to_real(); }
Value   Value::to_string() const { return Value(data->to_string()); }

Value operator+( Integer lhs, Value rhs ) { return Value(lhs) + rhs; }
Value operator+( Real lhs, Value rhs )    { return Value(lhs) + rhs; }
Value operator+( const char* lhs, Value rhs ) { return Value(lhs) + rhs; }
Value operator+( Logical lhs, Value rhs ) { return Value(lhs) + rhs; }
Value operator-( Integer lhs, Value rhs ) { return Value(lhs) - rhs; }
Value operator-( Real lhs, Value rhs )    { return Value(lhs) - rhs; }
Value operator-( Logical lhs, Value rhs ) { return Value(lhs) - rhs; }
Value operator*( Integer lhs, Value rhs ) { return Value(lhs) * rhs; }
Value operator*( Real lhs, Value rhs )    { return Value(lhs) * rhs; }
Value operator*( Logical lhs, Value rhs ) { return Value(lhs) * rhs; }
Value operator/( Integer lhs, Value rhs ) { return Value(lhs) / rhs; }
Value operator/( Real lhs, Value rhs )    { return Value(lhs) / rhs; }
Value operator/( Logical lhs, Value rhs ) { return Value(lhs) / rhs; }
Value operator%( Integer lhs, Value rhs ) { return Value(lhs) % rhs; }
Value operator%( Real lhs, Value rhs )    { return Value(lhs) % rhs; }
Value operator%( Logical lhs, Value rhs ) { return Value(lhs) % rhs; }

Value JSON( const char* json )
{
  return JSONParser(Value(json)).parse_value();
}

Value JSON( Character* json, Integer count )
{
  return JSONParser(Value(json,count)).parse_value();
}

Value List( Integer initial_capacity )
{
  return Value( new ListData(initial_capacity) );
}

Value Table( Integer bin_count )
{
  return Value( new TableData(bin_count) );
}

//=============================================================================
//  Data
//=============================================================================
Data::Data()
{
  reference_count = 0;
}

Data::~Data()
{
}

Data* Data::from( Integer i1, Integer i2 )
{
  return &null_singleton;
}

Data* Data::get( Value key )
{
  return &null_singleton;
}

Data* Data::get( Integer index )
{
  return &null_singleton;
}

Integer Data::compare_to( Data* other )
{
  Type t1 = type();
  Type t2 = other->type();
  if (t1 != t2)
  {
    return (t1 < t2) ? -1 : 1;
  }
  
  return 0;
}

Value Data::remove( Value key )
{
  return Value();
}

Value Data::remove_at( Integer index )
{
  return Value();
}

Data* Data::retain()
{
  ++reference_count;
  return this;
}

Data* Data::release()
{
  if (--reference_count == 0) delete this;

  return NULL;
}

Data* Data::to_json()
{
  StringBuilder buffer;
  JSONPrinter printer( &buffer );
  printer.print( Value(this) );
  return buffer.to_string();
}

Data* Data::to_string()
{
  StringBuilder buffer;
  Printer printer( &buffer );
  printer.print( Value(this) );
  return buffer.to_string();
}


//=============================================================================
//  NullData
//=============================================================================
Data* NullData::to_json()
{
  return &null_string_singleton;
}


Data* NullData::to_string()
{
  return &null_string_singleton;
}


//=============================================================================
//  False
//=============================================================================
Data* False::to_json()
{
  return &false_string_singleton;
}

Data* False::to_string()
{
  return &false_string_singleton;
}


//=============================================================================
//  True
//=============================================================================
Data* True::to_json()
{
  return &true_string_singleton;
}

Data* True::to_string()
{
  return &true_string_singleton;
}


//=============================================================================
//  Number
//=============================================================================
Number::Number( Real value ) : value(value)
{
}

Integer Number::compare_to( Data* other )
{
  if (other->type() != CARGO_NUMBER) return Data::compare_to(other);

  Real diff = value - ((Number*)other)->value;
  if (diff > 0) return  1;
  if (diff < 0) return -1;
  return 0;
}

Integer Number::hash_code()
{
  Long bits = *((Long*)&value);
  return (Integer)((bits >> 32LL) ^ bits);
}


void Number::to_c_string( Real value, char* buffer, int size )
{
  if (isnan(value) || value == INFINITY || value == -INFINITY)
  {
    // JSON doesn't have representations for +/-infinity and nan.
    snprintf( buffer, size, "null" );
  }
  else if (floor(value) == value)
  {
    if (value > -1e17 and value < 1e17)
    {
      // Whole number with 16 or fewer digits of precision
      snprintf( buffer, size, "%.0lf", value );
    }
    else
    {
      // Whole number with up to 17+ digits of precision, general format.
      snprintf( buffer, size, "%.16lg", value );
    }
  }
  else if (value > -0.1 and value < 0.1)
  {
    // +/- 0.0xxx - use the long general format (g) specifying 17 digits of
    // decimal precision which will result in either a standard decimal value
    // (%lf) or a value in exponent notation (%le), each with UP TO 17 digits
    // of precision (it may be fewer with %g!).
    snprintf( buffer, size, "%.17lg", value );
  }
  else
  {
    // +/- x.xxxx
    snprintf( buffer, size, "%.17lf", value );

    // Round off
    int len = strlen( buffer );
    if (buffer[len-1] < '5')
    {
      buffer[--len] = 0;
    }
    else
    {
      Logical carry = true;
      buffer[--len] = 0;
      int i = len - 1;
      while (i >= 0)
      {
        char ch = buffer[i];
        if (ch >= '0' && ch <= '9')
        {
          if (ch == '9')
          {
            buffer[i] = '0';
          }
          else
          {
            ++buffer[i];
            carry = false;
            break;
          }
        }
        --i;
      }
      if (carry)
      {
        // Need one more '1' at the front of all this
        i = -1;
        for (;;)
        {
          char ch = buffer[++i];
          if (ch >= '0' && ch <= '9') break;
        }
        memmove( buffer+i+1, buffer+i, len );
        ++len;
        buffer[i] = '1';
      }
    }

    // rescan the rounded-off value
    sscanf( buffer, "%lf", &value );

    // Remove excess digits as long as the reparsed value doesn't change.
    // Without doing this step 3.1415 would be printed as
    // 3.14150000000000018, for instance.
    //
    // With this process we see:
    //   3.14150000000000018 == 3.1415?  TRUE
    //   3.1415000000000001  == 3.1415?  TRUE
    //   3.141500000000000   == 3.1415?  TRUE
    //   ...
    //   3.1415              == 3.1415?  TRUE
    //   3.141               == 3.1415?  FALSE - add the 5 back on and done
    char temp = 0;
    while (len > 0)
    {
      temp = buffer[--len];  // store new temp
      buffer[len] = 0;
      Real n;
      sscanf( buffer, "%lf", &n );
      if (n != value) break;
    }

    // Put back the last character whose absence changed the value.
    buffer[len] = temp;
  }
}

//=============================================================================
//  String
//=============================================================================
String::String( const char* utf8, Integer len )
  : hash_computed(false)
{
  if (len == -1) len = strlen( utf8 );

  Integer decoded_count = 0;
  Integer i;
  for (i=0; i<len; ++i)
  {
    ++decoded_count;
    Integer ch = ((const unsigned char*) utf8)[ i ];
    if (ch >= 0x80)
    {
      if ((ch & 0xe0) == 0xc0)      ++i;
      else if ((ch & 0xf0) == 0xe0) i += 2;
      else                          i += 3;
    }
  }

  if (i > len)
  {
    // This happens if the string ends with only the first part of a multi-byte character encoding.
    len -= (i - len); // Stop before the offending character
  }

  character_data = new Character[ decoded_count+1 ];
  length = decoded_count;

  Byte* src = (Byte*) (utf8 - 1);
  Character* dest = character_data - 1;
  while (--len >= 0)
  {
    Integer ch = *(++src);
    if (ch >= 0x80)
    {
      if ((ch & 0xe0) == 0xc0)
      {
        // 110x xxxx  10xx xxxx
        if (--len >= 0)
        {
          ch = ((ch & 0x1f) << 6) | (*(++src) & 0x3f);
        }
      }
      else if ((ch & 0xf0) == 0xe0)
      {
        // 1110 xxxx  10xx xxxx  10xx xxxx
        if ((len -= 2) >= 0)
        {
          ch = ((ch & 0x1f) << 6) | (*(++src) & 0x3f);
          ch = (ch << 6) | (*(++src) & 0x3f);
        }
      }
      else
      {
        // 11110xxx 	10xxxxxx 	10xxxxxx 	10xxxxxx
        if ((len -= 3) >= 0)
        {
          ch = ((ch & 7) << 18) | ((src[1] & 0x3f) << 12);
          ch |= (src[2] & 0x3f) << 6;
          ch |= (src[3] & 0x3f);
          src += 3;
        }
        if (ch >= 0x10000)
        {
          // write surrogate pair
          ch -= 0x10000;
          dest[1] = (Character) (0xd800 + ((ch >> 10) & 0x3ff));
          dest[2] = (Character) (0xdc00 + (ch & 0x3ff));
          dest += 2;
          continue;
        }
        // else fall through and write a regular character
      }
    }
    *(++dest) = (Character) ch;
  }

  *(++dest) = 0;  // null terminate for compatibility with 16-bit WCHAR strings
}

String::String( Character* data, Integer len )
  : hash_computed(false)
{
  length = len;
  character_data = new Character[ length+1 ];
  memcpy( character_data, data, length*sizeof(Character) );

  // null terminate for compatibility with 16-bit WCHAR strings
  character_data[length] = 0;
}

String::String( Integer length )
  : hash_computed(false)
{
  this->length = length;
  character_data = new Character[ length + 1 ];
  character_data[ length ] = (Character) 0;
}

String::~String()
{
  delete character_data;
}

Character String::character_at( Integer index )
{
  if ((unsigned int)index >= (unsigned int)length) return 0;
  return character_data[ index ];
}

Integer String::compare_to( const char* other )
{
  Integer other_count = strlen( other );
  Integer min_count = (length <= other_count) ? length : other_count;

  for (Integer i=0; i<min_count; ++i)
  {
    Integer diff;
    if ((diff = (character_data[i] - other[i])))
    {
      if (diff < 0) return -1;
      else          return  1;
    }
  }

  if (length == other_count) return 0;
  if (length < other_count)  return -1;

  return 1;
}

Integer String::compare_to( Data* other )
{
  if (other->type() != CARGO_STRING) return Data::compare_to( other );

  String* st2 = (String*) other;

  Integer other_count = st2->length;
  Integer min_count = (length <= other_count) ? length : other_count;
  Character* other_characters = st2->character_data;

  for (Integer i=0; i<min_count; ++i)
  {
    Integer diff;
    if ((diff = (character_data[i] - other_characters[i])))
    {
      if (diff < 0) return -1;
      else          return  1;
    }
  }

  if (length == other_count) return 0;
  if (length < other_count)  return -1;

  return 1;
}

Data* String::from( Integer i1, Integer i2 )
{
  if (i1 < 0) i1 = 0;
  if (i2 >= length) i2 = length - 1;
  if (i1 == 0 && i2 == length-1) return this;
  if (i1 > i2) return &empty_string_singleton;

  return new String( character_data+i1, (i2-i1)+1 );
}

Data* String::get( Integer index )
{
  if ((unsigned int)index >= (unsigned int)length)
  {
    return &empty_string_singleton;
  }

  return new String( character_data+index, 1 );
}

Integer String::hash_code()
{
  if ( !hash_computed )
  {
    Integer code = 0;
    Integer len = length;
    Character* data = character_data - 1;
    while (--len >= 0)
    {
      // code = code * 7 + ch
      code = ((code << 3) - code) + (Integer) *(++data);
    }

    hash = code;
    hash_computed = true;
  }

  return hash;
}

Integer String::locate( Value value, Integer i1 )
{
  if ( !value.is_string() ) return -1;

  switch (value.count())
  {
    case 0: return -1;

    case 1:
    {
      Character ch = value.character_at(0);
      Integer limit = length;
      for (Integer i=i1; i<limit; ++i)
      {
        if (character_data[i] == ch) return i;
      }
      return -1;
    }

    default:
    {
      Integer    other_count      = value.count();
      Character* other_characters = ((String*)(value.data))->character_data;

      Character ch  = value.character_at(0);
      Integer limit = length - (other_count - 1);
      for (Integer i=i1; i<limit; ++i)
      {
        if (character_data[i] == ch)
        {
          Logical matches = true;
          for (Integer j=1; j<other_count; ++j)
          {
            if (other_characters[j] != character_data[i+j])
            {
              matches = false;
              break;
            }
          }
          if (matches) return i;
        }
      }
      return -1;
    }
  }
}

Integer String::locate_last( Value value, Integer i1 )
{
  if ( !value.is_string() ) return -1;

  switch (value.count())
  {
    case 0: return -1;

    case 1:
    {
      Character ch = value.character_at(0);
      if (i1 >= length) i1 = length - 1;
      for (Integer i=i1+1; --i>=0; )
      {
        if (character_data[i] == ch) return i;
      }
      return -1;
    }

    default:
    {
      Integer    other_count      = value.count();
      Character* other_characters = ((String*)(value.data))->character_data;

      Character ch  = value.character_at(0);
      Integer limit = length - (other_count - 1);
      if (i1 >= limit) i1 = limit - 1;
      for (Integer i=i1+1; --i>=0;)
      {
        if (character_data[i] == ch)
        {
          Logical matches = true;
          for (Integer j=1; j<other_count; ++j)
          {
            if (other_characters[j] != character_data[i+j])
            {
              matches = false;
              break;
            }
          }
          if (matches) return i;
        }
      }
      return -1;
    }
  }
}

Data* String::to_string()
{
  return this;
}

char* String::to_utf8_string( char* buffer, Integer buffer_size )
{
  // Count number of bytes that will be required.
  Integer encoded_count = utf8_count();

  if ( !buffer )
  {
    buffer_size = encoded_count + 1;
    buffer = new char[ buffer_size ];
  }

  --buffer_size;  // leave room for null terminator

  Integer copy_count = length;
  if (buffer_size < encoded_count)
  {
    // Not enough room.  Search through the data one last time to find the
    // number of characters we can copy and still fit.
    copy_count = 0;
    Integer copy_size = 0;
    for (int i=0; i<length; ++i)
    {
      ++copy_count;
      int ch = character_data[i];
      if (ch >= 0x80)
      {
        if (ch >= 0x800)
        {
          if ((copy_size+=3) > buffer_size) break;
        }
        else if (ch >= 0xd800 && ch <= 0xdfff)
        {
          if ((copy_size+=4) > buffer_size) break;
          ++i;
        }
        else
        {
          if ((copy_size+=2) > buffer_size) break;
        }
      }
      else
      {
        if (++copy_size > buffer_size) break;
      }
    }
  }

  char* dest = buffer - 1;
  for (int i=0; i<copy_count; ++i)
  {
    int ch = character_data[i];
    if (ch <= 0x7f)
    {
      //0..0x7f (0-127) is written normally
      *(++dest) = (char) ch;
    }
    else if (ch <= 0x7ff)
    {
      //0x80..0x7ff is encoded in two bytes.
      //0000 0xxx yyyy yyyy -> 110xxxyy 10yyyyyy

      //top_5    = (1100 0000 | ((ch>>6) & 1 1111))
      int top_5    = (0xc0      | ((ch>>6) & 0x1f));

      //bottom_6 = (1000 0000 | (ch & 11 1111))
      int bottom_6 = (0x80      | (ch & 0x3f));

      *(++dest) = (char) top_5;
      *(++dest) = (char) bottom_6;
    }
    else if (ch >= 0xd800 && ch <= 0xdfff)
    {
      // 'ch' is the high part of a surrogate pair
      int low = character_data[++i];
      if (low >= 0xdc00 && low <= 0xdfff)
      {
        // Get the full 20 bit character value
        int value = 0x10000 + (((ch - 0xd800)<<10) | (low-0xdc00));

        // Write it out in 4 UTF8 bytes.
        dest[0] = (char) (0xf0 | ((value>>18) & 7));
        dest[1] = (char) (0x80 | ((value>>12) & 0x3f));
        dest[2] = (char) (0x80 | ((value>>6)  & 0x3f));
        dest[3] = (char) ((value & 0x3f) | 0x80);
        dest += 4;
      }
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

      *(++dest) = (char) top_4;
      *(++dest) = (char) mid_6;
      *(++dest) = (char) bottom_6;
    }
  }

  buffer[encoded_count] = 0;  // null terminate
  return buffer;
}

Integer String::to_integer()
{
  char buffer[80];
  Integer value;
  sscanf( to_utf8_string(buffer,80), "%d", &value );
  return value;
}

Logical String::to_logical()
{
  if (compare_to("true") == 0)  return true;
  if (compare_to("false") == 0) return false;
  return to_integer() != 0;
}

Real String::to_real()
{
  char buffer[80];
  Real value;
  sscanf( to_utf8_string(buffer,80), "%lf", &value );
  return value;
}

Integer String::utf8_count()
{
  // Returns the number of bytes this string will occupy when encoded as UTF8.
  int n = 0;
  int len = length;

  for (Integer i=0; i<len; ++i)
  {
    int ch = character_data[i];
    if (ch < 0x80)
    {
      ++n;
    }
    else if (ch < 0x800)
    {
      n += 2;
    }
    else if (ch >= 0xd800 && ch <= 0xdfff)
    {
      // start of a surrogate pair
      n += 4;
      ++i;  // skip the next character
    }
    else
    {
      n += 3;
    }
  }

  return n;
}

//=============================================================================
//  ListData
//=============================================================================
ListData::ListData( Integer initial_capacity )
    : capacity(initial_capacity), element_count(0)
{
  data = new Value[ initial_capacity ];
}

ListData::~ListData()
{
  delete[] data;
}

void ListData::add( Data* value )
{
  reserve( 1 );
  data[ element_count++ ] = Value(value);
}

void ListData::clear()
{
  // Need to release references on list items.
  while (element_count)
  {
    data[ --element_count ] = Value();  // set to null value
  }
}

Data* ListData::clone()
{
  ListData* result = new ListData( element_count );
  for (Integer i=0; i<element_count; ++i) result->add( data[i].clone().data );
  return result;
}

Logical ListData::contains( Value value )
{
  for (Integer i=element_count; --i>=0; )
  {
    if (data[i] == value) return true;
  }
  return false;
}

Data* ListData::from( Integer i1, Integer i2 )
{
  if (i1 < 0) i1 = 0;
  if (i2 >= element_count) i2 = element_count - 1;
  if (i1 == 0 && i2 == element_count-1) return this;
  if (i1 > i2) return new ListData(1);

  ListData* result = new ListData( (i2-i1)+1 );
  for (Integer i=i1; i1<=i2; ++i)
  {
    result->add( get(i) );
  }

  return result;
}

Data* ListData::get( Integer index )
{
  if ((unsigned int)index >= (unsigned int)element_count) return &null_singleton;
  return data[ index ].data;
}

void ListData::insert( Data* value )
{
  reserve( 1 );
  for (Integer i=element_count; i>0; --i)
  {
    data[ i ] = data[ i-1 ];
  }
  data[0] = value;
  ++element_count;
}

Integer ListData::locate( Value value, Integer i1 )
{
  if (i1 < 0) i1 = 0;
  Integer limit = element_count;
  for (Integer i=i1; i<limit; ++i)
  {
    if (data[i] == value) return i;
  }
  return -1;
}

Integer ListData::locate_last( Value value, Integer i1 )
{
  if (i1 >= element_count) i1 = element_count - 1;

  for (Integer i=i1; --i>=0; )
  {
    if (data[i] == value) return i;
  }
  return -1;
}

void ListData::set( Value key, Value value )
{
  Integer index = key.to_integer();

  if (index >= 0)
  {
    // Add nulls to expand the list to the proper size
    while (index >= element_count) add( &null_singleton );

    data[index] = value;
  }
}

Value ListData::remove( Value key )
{
  for (int i=count(); --i>=0; )
  {
    if (0 == get(i)->compare_to(key.data)) return remove_at( i );
  }
  return Value();
}

Value ListData::remove_at( Integer index )
{
  if ((unsigned int)index >= (unsigned int)element_count) return Value();

  Value result = data[ index ];

  Integer limit = --element_count;
  while (index < limit)
  {
    data[index] = data[index+1];
    ++index;
  }

  return result;
}

void ListData::reserve( Integer additional_capacity )
{
  Integer required_capacity = element_count + additional_capacity;
  if (capacity >= required_capacity) return;

  Integer double_capacity = capacity * 2;
  if (double_capacity > required_capacity) required_capacity = double_capacity;

  Value* new_data = new Value[ required_capacity ];
  for (Integer i=element_count; --i>=0; )
  {
    new_data[i] = data[i];
  }

  delete[] data;
  data = new_data;
  capacity = required_capacity;
}

Integer ListData::to_integer()
{
  return element_count;
}

Logical ListData::to_logical()
{
  return element_count > 0;
}

Real ListData::to_real()
{
  return to_integer();
}


//=============================================================================
//  TableEntry
//=============================================================================
TableEntry::TableEntry( Value key, Value value, Integer hash_code, TableEntry* next_entry )
    : key(key), value(value), hash_code(hash_code), next_entry(next_entry)
{
}


//=============================================================================
//  TableData
//=============================================================================
TableData::TableData( Integer initial_capacity )
  : element_count(0)
{
  Integer bin_count = initial_capacity / 2;  // Load factor 2

  // Ensure bin_count is a pointer of 2.
  Integer bc = 1;
  while (bc < bin_count) bc <<= 1;
  bin_count = bc;
  this->bin_count = bin_count;

  bins = new TableEntry*[ bin_count ];
  memset( bins, 0, bin_count*sizeof(TableEntry*) );

  bin_mask = (bin_count - 1);

  // Resize when there are 2 elements per bin on average (load factor 2).
  capacity = bin_count * 2;
}

TableData::~TableData()
{
  if (bins)
  {
    clear();
    delete bins;
  }
}

Data* TableData::at( Integer index )
{
  if ((unsigned int)index >= (unsigned int)element_count) return &null_singleton;

  for (int b=0; b<bin_count; ++b)
  {
    TableEntry* cur = bins[ b ];
    while (cur)
    {
      if (--index == -1) return cur->key.data;
      cur = cur->next_entry;
    }
  }

  return &null_singleton;
}

void TableData::clear()
{
  for (int b=0; b<bin_count; ++b)
  {
    TableEntry* cur = bins[ b ];
    while (cur)
    {
      TableEntry* next = cur->next_entry;
      delete cur;
      cur = next;
    }
    bins[ b ] = 0;
  }
  element_count = 0;
}

Data* TableData::clone()
{
  TableData* new_data = new TableData( bin_count );
  for (int b=0; b<bin_count; ++b)
  {
    TableEntry* cur = bins[ b ];
    while (cur)
    {
      new_data->set( cur->key, cur->value.clone() );
      cur = cur->next_entry;
    }
  }
  return new_data;
}

TableEntry* TableData::find( Value key )
{
  Integer hash = key.hash_code();
  TableEntry* cur = bins[ hash & bin_mask ];

  while (cur)
  {
    if (cur->hash_code == hash && cur->key == key)
    {
      return cur;
    }
    cur = cur->next_entry;
  }

  return 0;
}

Data* TableData::get( Integer index )
{
  return get( Value(index) );
}

Data* TableData::get( Value key )
{
  TableEntry* entry = find( key );
  if (entry) return entry->value.data;
  else       return &null_singleton;
}

Value TableData::remove( Value key )
{
  TableEntry* entry = find( key );
  if ( !entry ) return Value();

  return remove_entry( entry );
}

Value TableData::remove_at( Integer index )
{
  if ((unsigned int)index >= (unsigned int)element_count) return Value();

  for (int b=0; b<bin_count; ++b)
  {
    TableEntry* cur = bins[ b ];
    while (cur)
    {
      if (--index == -1) return remove_entry( cur );
      cur = cur->next_entry;
    }
  }

  return Value();  // logically unreachable
}

Value TableData::remove_entry( TableEntry* entry )
{
  Value result = entry->value;

  int bindex = entry->hash_code & bin_mask;
  if (bins[bindex] == entry)
  {
    // Entry to remove is head of list.
    bins[ bindex ] = entry->next_entry;
    delete entry;
  }
  else
  {
    TableEntry* cur = bins[ bindex ];
    while (cur->next_entry != entry) cur = cur->next_entry;

    TableEntry* next_entry = cur->next_entry->next_entry;
    delete cur->next_entry;
    cur->next_entry = next_entry;
  }

  --element_count;
  return result;
}

void TableData::reorder()
{
  // New table elements are inserted at the beginning of each bin, causing bin elements
  // to reverse their order every time a table is saved and then loaded.  To prevent
  // this "thrashing", JSONParser calls reorder() on every table that's loaded to reverse
  // its bin order.
  for (int b=0; b<bin_count; ++b)
  {
    TableEntry* reordered = 0;
    TableEntry* cur = bins[ b ];
    while (cur)
    {
      TableEntry* next = cur->next_entry;
      cur->next_entry = reordered;
      reordered = cur;
      cur = next;
    }
    bins[ b ] = reordered;
  }
}

void TableData::set( Value key, Value value )
{
  TableEntry* entry = find( key );
  if (entry)
  {
    entry->value = value;
  }
  else
  {
    if (element_count >= capacity)
    {
      // Create new table with 4x the capacity.
      Value new_table = Table( capacity * 4 );

      // Copy over all the elements
      for (int b=0; b<bin_count; ++b)
      {
        TableEntry* cur = bins[ b ];
        while (cur)
        {
          new_table.set( cur->key, cur->value );
          cur = cur->next_entry;
        }
      }

      // Replace this table's data with the new table data.
      clear();
      delete bins;

      TableData* new_data = (TableData*)(new_table.data);

      bins = new_data->bins;
      bin_count = new_data->bin_count;
      bin_mask  = new_data->bin_mask;
      element_count = new_data->element_count;
      capacity = new_data->capacity;

      new_data->bins = 0;  // Prevent work table from freeing the data we just adopted.
    }

    Integer hash = key.hash_code();
    Integer bindex  = hash & bin_mask;
    TableEntry* cur = new TableEntry( key, value, hash, bins[bindex] );
    bins[bindex] = cur;
    ++element_count;
  }
}

Integer TableData::to_integer()
{
  return element_count;
}

Logical TableData::to_logical()
{
  return element_count > 0;
}

Real TableData::to_real()
{
  return to_integer();
}


//=============================================================================
//  NativeData
//=============================================================================
void FreeOnRelease( void* data )
{
  free( data );
}

NativeData::NativeData( void* data, NativeDataReleaseFn release_fn )
  : data(data), release_fn(release_fn)
{
}

NativeData::~NativeData()
{
  if (release_fn) release_fn( data );
}

Integer NativeData::compare_to( Data* other )
{
  if (other->type() != CARGO_NATIVE_DATA) return Data::compare_to(other);

  intptr_t d1 = (intptr_t) data;
  intptr_t d2 = (intptr_t) (((NativeData*)other)->data);
  if (d1 < d2) return -1;
  if (d1 > d2) return  1;
  return 0;
}

//=============================================================================
//  File
//=============================================================================
File::File( Value filepath )
{
  filepath = filepath.to_string();
  if (filepath.character_at(0) == '~')
  {
    filepath = home_folder() + filepath.from(1);
  }
  this->filepath = filepath;
}

File::File( Value filepath, Value name )
{
#if defined(_WIN32)
  this->filepath = File(filepath+"\\"+name).filepath;
#else
  this->filepath = File(filepath+"/"+name).filepath;
#endif
}

File::File( const File& other )
{
  filepath = other.filepath;
}

File& File::operator=( const File& other )
{
  filepath = other.filepath;
  return *this;
}

Value File::absolute_filepath()
{
  char c_filepath[PATH_MAX+3];
  filepath.to_c_string( c_filepath, PATH_MAX );

#if defined(_WIN32)
  {
    char long_name[PATH_MAX+4];
    char full_name[PATH_MAX+4];

    if (GetLongPathName(c_filepath,long_name,PATH_MAX+4) == 0)
    {
      strcpy_s( long_name, PATH_MAX+4, c_filepath );
    }

    if (GetFullPathName(long_name,PATH_MAX+4,full_name,0) == 0)
    {
      // bail with filepath unchanged
      return filepath;
    }

    return Value( full_name );
  }
#else
  {
    Integer original_dir_fd;
    Integer new_dir_fd;
    char filename[PATH_MAX];

    Logical is_folder = this->is_folder();

    // A way to get back to the starting folder when finished.
    original_dir_fd = open( ".", O_RDONLY );  

    if (is_folder)
    {
      filename[0] = 0;
    }
    else
    {
      // fchdir only works with a path, not a path+filename (c_filepath).
      // Copy out the filename and null terminate the c_filepath to be just a path.
      int i = (int) strlen( c_filepath ) - 1;
      while (i >= 0 && c_filepath[i] != '/') --i;
      strcpy( filename, c_filepath+i+1 );
      c_filepath[i] = 0;
    }
    new_dir_fd = open( c_filepath, O_RDONLY );

    if (original_dir_fd >= 0 && new_dir_fd >= 0)
    {
      fchdir( new_dir_fd );
      getcwd( c_filepath, PATH_MAX );
      if ( !is_folder ) 
      {
        strcat( c_filepath, "/" );
        strcat( c_filepath, filename );
      }
      fchdir( original_dir_fd );
    }
    if (original_dir_fd >= 0) close( original_dir_fd );
    if (new_dir_fd >= 0) close( new_dir_fd );

    return Value( c_filepath );
  }
#endif
}

Value File::base_filename()
{
  Value name = filename();
  Integer i = name.locate_last( "." );
  if (i == -1) return name;
  return name.from( 0, i-1 );
}

Logical File::exists()
{
  char c_filepath[4096];
  filepath.to_c_string( c_filepath, sizeof(c_filepath) );

  FILE* fp = fopen( c_filepath, "rb" );
  if ( !fp ) return false;

  fclose( fp );
  return true;
}

Value File::extension()
{
  Value name = filename();
  Integer i = name.locate_last(".");
  if (i != -1) return name.from(i+1);
  else         return "";
}

Value File::filename()
{
  Integer i1 = filepath.locate_last( "/" );
  Integer i2 = filepath.locate_last( "\\" );
  if (i2 > i1) i1 = i2;

  if (i1 != -1) return filepath.from( i1+1 );
  else          return filepath;
}

Logical File::is_folder()
{
  char c_filepath[ PATH_MAX ];
  filepath.to_c_string( c_filepath, PATH_MAX );

#if defined(_WIN32)
  int path_len = strlen( c_filepath );
  int i = path_len - 1;
  while (i > 0 && (c_filepath[i] == '/' || c_filepath[i] == '\\')) c_filepath[i--] = 0;

  // Windows allows filepaths with wildcards to count as a directory; guard against.
  for (i=0; c_filepath[i]; ++i)
  {
    if (c_filepath[i] == '*' || c_filepath[i] == '?') return false;
  }

  WIN32_FIND_DATA entry;
  HANDLE dir = FindFirstFile( c_filepath, &entry );
  if (dir != INVALID_HANDLE_VALUE)
  {
    if (entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      FindClose( dir );
      return true;
    }
  }
  FindClose( dir );
  return false;

#else
  DIR* dir = opendir( c_filepath );
  if ( !dir ) return false;

  closedir( dir );
  return true;
#endif
}

Value File::listing()
{
  Value result = List();

#if defined(_WIN32)
  {
    // Add a \* to the end of the filename for windows.
    Value filepath = this->filepath + "\\*";

    char c_filepath[4096];
    filepath.to_c_string( c_filepath, sizeof(c_filepath) );

    WIN32_FIND_DATA entry;
    HANDLE dir = FindFirstFile( c_filepath, &entry );

    if (dir != INVALID_HANDLE_VALUE)
    {
      do
      {
        // Don't include '.' or '..'
        int keep = 1;
        if (entry.cFileName[0] == '.')
        {
          switch (entry.cFileName[1])
          {
            case 0:   
              keep = 0; 
              break;
            case '.':
              keep = entry.cFileName[2] != 0;
              break;
          }
        }
        if (keep)
        {
          result.add( entry.cFileName );
        }
      }
      while (FindNextFile(dir,&entry));

      FindClose( dir );
    }
  }

#else
  // UNIX
  {
    DIR* dir;
    struct dirent* entry;

    char c_filepath[4096];
    filepath.to_c_string( c_filepath, sizeof(c_filepath) );

    dir = opendir( c_filepath );
    if (dir)
    {
      entry = readdir( dir );
      while (entry != NULL)
      {
        // Don't include '.' or '..'
        int keep = 1;
        if (entry->d_name[0] == '.')
        {
          switch (entry->d_name[1])
          {
            case 0:   
              keep = 0; 
              break;
            case '.':
              keep = entry->d_name[2] != 0;
              break;
          }
        }
        if (keep)
        {
          result.add( entry->d_name );
        }
        entry = readdir( dir );
      }
      closedir( dir );
    }
  }
#endif

  return result;
}

Value File::load_json()
{
  if ( !exists() ) return Value();

  return JSONParser(*this).parse_value();
}

Value File::load_list()
{
  Value value = load_json();
  if ( !value.is_list() ) return List();
  else                    return value;
}

Value File::load_table()
{
  Value value = load_json();
  if ( !value.is_table() ) return Table();
  else                     return value;
}

Value File::path()
{
  Integer i1 = filepath.locate_last( '\\' );
  Integer i2 = filepath.locate_last( '/' );
  if (i1 < i2) i1 = i2;
  if (i1 == -1) return Value("");
  return filepath.from( 0,i1-1 );
}


FILE* File::reader()
{
  char c_filepath[4096];
  filepath.to_c_string( c_filepath, sizeof(c_filepath) );

  return fopen( c_filepath, "rb" );
}

bool File::save( Value value, Logical force_ascii )
{
  JSONPrinter printer( *this, force_ascii );
  if (printer.error()) return false;

  printer.print( value );
  printer.close();

  return true;
}

Long File::size()
{
  char c_filepath[4096];
  filepath.to_c_string( c_filepath, sizeof(c_filepath) );

  struct stat st;
  stat( c_filepath, &st );
  return (Long) st.st_size;
}

FILE* File::writer()
{
  char c_filepath[4096];
  filepath.to_c_string( c_filepath, sizeof(c_filepath) );

  return fopen( c_filepath, "wb" );
}

Value   File::absolute_filepath( Value filepath )
{
  return File(filepath).absolute_filepath();
}

Value File::base_filename( Value filepath )
{
  return File(filepath).base_filename();
}

Logical File::exists( Value filepath )
{
  return File(filepath).exists();
}

Value   File::home_folder()
{
  return Value( getenv("HOME") );
}

Value File::extension( Value filepath )
{
  return File(filepath).extension();
}

Value File::filename( Value filepath )
{
  return File(filepath).filename();
}

Logical File::is_folder( Value filepath )
{
  return File(filepath).is_folder();
}

Value File::listing( Value filepath )
{
  return File(filepath).listing();
}

Value   File::load_json( Value filepath )
{
  return File(filepath).load_json();
}

Value   File::load_list( Value filepath )
{
  return File(filepath).load_list();
}

Value   File::load_table( Value filepath )
{
  return File(filepath).load_table();
}

Value   File::path( Value filepath )
{
  return File(filepath).path();
}

bool    File::save( Value filepath, Value value, Logical force_ascii )
{
  return File(filepath).save( value, force_ascii );
}

Long File::size( Value filepath )
{
  return File(filepath).size();
}


//=============================================================================
//  Printer
//=============================================================================
Printer::Printer()
  : force_ascii(false), at_newline(true), indent(0)
{
  writer = stdout_singleton.retain();
}

Printer::Printer( const Printer& other )
{
  writer = other.writer->retain();
  force_ascii = other.force_ascii;
  at_newline = other.at_newline;
  indent = other.indent;
}

Printer::Printer( File file, Logical force_ascii )
  : force_ascii(force_ascii), at_newline(true), indent(0)
{
  writer = (new FileWriter(file))->retain();
}

Printer::Printer( StringBuilder* builder )
  : force_ascii(false), at_newline(true), indent(0)
{
  writer = builder->writer.retain();
}

Printer::~Printer()
{
  writer->release();
}

Printer& Printer::operator=( const Printer& other )
{
  other.writer->retain();
  writer->release();
  writer = other.writer;
  force_ascii = other.force_ascii;
  at_newline = other.at_newline;
  indent = other.indent;
  return *this;
}

void Printer::close()
{
  writer->flush();
  writer->close();
}

Logical Printer::error()
{
  return writer->error();
}

void Printer::flush()
{
  writer->flush();
}

void Printer::print( Character code )
{
  if (code == '\n')
  {
    at_newline = true;
  }
  else if (at_newline)
  {
    at_newline = false;
    for (Integer i=0; i<indent; ++i)
    {
      writer->write( ' ' );
    }
  }

  writer->write( code );
}

void Printer::print( const char* c_string )
{
  Character ch;
  for (--c_string; (ch=*(++c_string)); )
  {
    print( (Character)ch );
  }
}

void Printer::print( Value value, Logical quote_strings )
{
  if (value.is_null())
  {
    print( "null" );
  }
  else if (value.is_logical())
  {
    if (value.to_logical()) print( "true" );
    else                    print( "false" );
  }
  else if (value.is_number())
  {
    char st[512];
    Number::to_c_string( value.to_real(), st, 512 );
    print( st );
  }
  else if (value.is_string())
  {
    if (quote_strings) print( '"' );
    Integer count = value.count();
    Character* characters = value.characters();
    for (Integer i=0; i<count; ++i)
    {
      print( characters[i] );
    }
    if (quote_strings) print( '"' );
  }
  else if (value.is_list())
  {
    if (value.count() == 0)
    {
      print( "{}" );
    }
    else
    {
      if ( !at_newline ) println();
      println( '[' );
      indent += 2;
      Integer count = value.count();
      for (Integer i=0; i<count; ++i)
      {
        print( value.get(i), true );
        if (i+1 < count) print( "," );
        println();
      }
      indent -= 2;
      print( ']' );
    }
  }
  else if (value.is_table())
  {
    if (value.count() == 0)
    {
      print( "{}" );
    }
    else
    {
      if ( !at_newline ) println();
      print( "{\n" );
      indent += 2;
      Integer i = 0;
      Integer count = value.count();
      Integer      bin_count = ((TableData*)(value.data))->bin_count;
      TableEntry** bins = ((TableData*)(value.data))->bins;
      for (Integer b=0; b<bin_count; ++b)
      {
        TableEntry* cur = bins[ b ];
        while (cur)
        {
          print( cur->key.to_string() );
          print( ":" );
          print( cur->value, true );
          cur = cur->next_entry;
          if (++i < count) print( "," );
          println();
        }
      }
      indent -= 2;
      print( "}" );
    }
  }
  else
  {
    print( "(Native Data)" );
  }
}

//=============================================================================
//  JSONPrinter
//=============================================================================
JSONPrinter::JSONPrinter()
  : force_ascii(false)
{
  writer = stdout_singleton.retain();
}

JSONPrinter::JSONPrinter( const JSONPrinter& other )
  : force_ascii(false)
{
  writer = other.writer->retain();
}

JSONPrinter::JSONPrinter( File file, Logical force_ascii )
  : force_ascii(force_ascii)
{
  writer = (new FileWriter(file))->retain();
}

JSONPrinter::JSONPrinter( StringBuilder* builder )
  : force_ascii(false)
{
  writer = builder->writer.retain();
}

JSONPrinter::~JSONPrinter()
{
  writer->release();
}

JSONPrinter& JSONPrinter::operator=( const JSONPrinter& other )
{
  other.writer->retain();
  writer->release();
  writer = other.writer;
  return *this;
}

void JSONPrinter::close()
{
  writer->flush();
  writer->close();
}

Logical JSONPrinter::error()
{
  return writer->error();
}

void JSONPrinter::flush()
{
  writer->flush();
}

void JSONPrinter::print( Character code )
{
  writer->write( code );
}

void JSONPrinter::print( const char* c_string )
{
  Character ch;
  for (--c_string; (ch=*(++c_string)); )
  {
    print( (Character)ch );
  }
}

void JSONPrinter::print( Value value )
{
  if (value.is_null())
  {
    print( "null" );
  }
  else if (value.is_logical())
  {
    if (value.to_logical()) print( "true" );
    else                    print( "false" );
  }
  else if (value.is_number())
  {
    char st[512];
    Number::to_c_string( value.to_real(), st, 512 );
    print( st );
  }
  else if (value.is_string())
  {
    print( '"' );
    Integer count = value.count();
    Character* characters = value.characters();
    for (Integer i=0; i<count; ++i)
    {
      Character ch = characters[ i ];
      switch (ch)
      {
        case '"':   print( "\\\"" ); break;
        case '\\':  print( "\\\\" ); break;
        case '/':   print( "\\/" ); break;
        case '\b':  print( "\\b" ); break;
        case '\f':  print( "\\f" ); break;
        case '\n':  print( "\\n" ); break;
        case '\r':  print( "\\r" ); break;
        case '\t':  print( "\\t" ); break;
        default:
          if (ch >= 32 && ch <= 126)
          {
            print( (Byte) ch );
          }
          else if (force_ascii || ch < 32 || ch == 127 || ch == 0x2028 || ch == 0x2029)
          {
            // RE: 0x2028/0x2029:
            // http://stackoverflow.com/questions/2965293/javascript-parse-error-on-u2028-unicode-character
            print( "\\u" );
            int n = ch;
            for (int nibble=0; nibble<4; ++nibble)
            {
              int digit = (n >> 12) & 15;
              n <<= 4;
              if (digit <= 9) print( (Byte)('0'+digit) );
              else            print( (Byte)('a' + (digit-10)) );
            }
          }
          else
          {
            // Leave as literal Unicode
            print( ch );
          }
      }
    }
    print( '"' );
  }
  else if (value.is_list())
  {
    print( "[" );
    Integer count = value.count();
    for (Integer i=0; i<count; ++i)
    {
      if (i > 0) print( "," );
      print( value.get(i) );
    }
    print( "]" );
  }
  else if (value.is_table())
  {
    print( "{" );
    Integer i = 0;
    Integer      bin_count = ((TableData*)(value.data))->bin_count;
    TableEntry** bins = ((TableData*)(value.data))->bins;
    for (Integer b=0; b<bin_count; ++b)
    {
      TableEntry* cur = bins[ b ];
      while (cur)
      {
        if (i++ > 0) print( "," );
        print( cur->key.to_string() );
        print( ":" );
        print( cur->value );
        cur = cur->next_entry;
      }
    }
    print( "}" );
  }
  else
  {
    print( "(Native Data)" );
  }
}

//=============================================================================
//  CharacterWriter
//=============================================================================


//=============================================================================
//  UTF8Writer
//=============================================================================
void UTF8Writer::write( Character code )
{
  if (code <= 0x7f)
  {
    //0..0x7f (0-127) is written normally
    write_byte( (Byte) code );
  }
  else if (code <= 0x7ff)
  {
    //0x80..0x7ff is encoded in two bytes.
    //0000 0xxx yyyy yyyy -> 110xxxyy 10yyyyyy

    //top_5    = (1100 0000 | ((code>>6) & 1 1111))
    int top_5    = (0xc0      | ((code>>6) & 0x1f));

    //bottom_6 = (1000 0000 | (code & 11 1111))
    int bottom_6 = (0x80      | (code & 0x3f));

    write_byte( (Byte) top_5 );
    write_byte( (Byte) bottom_6 );
  }
  else
  {
    //0x8000..0xffff is encoded in three bytes.
    //xxxx xxxx yyyy yyyy -> 1110xxxx 10xxxxyy 10yyyyyy

    //top_4    = (1110 0000 | ((code>>12) & 1111))
    int top_4    = (0xe0      | ((code>>12) & 0xf));

    //mid_6    = (1000 0000 | ((code>>6)  & 11 1111))
    int mid_6    = (0x80      | ((code>>6)  & 0x3f));

    //bottom_6 = (1000 0000 | (code & 11 1111))
    int bottom_6 = (0x80 | (code & 0x3f));

    write_byte( (Byte) top_4 );
    write_byte( (Byte) mid_6 );
    write_byte( (Byte) bottom_6 );
  }
}


//=============================================================================
//  StandardOutput
//=============================================================================
StandardOutput::StandardOutput()
{
  reference_count = 1;
}

void StandardOutput::write_byte( Byte value )
{
  fputc( value, stdout );
}

StandardOutput stdout_singleton;


//=============================================================================
//  FileWriter
//=============================================================================
FileWriter::FileWriter( File file )
  : buffer_count(0)
{
  fp = file.writer();
}

FileWriter::~FileWriter()
{
  if (fp) close();
}

void FileWriter::close()
{
  if (fp)
  {
    flush();
    fclose( fp );
    fp = 0;
  }
}

Logical FileWriter::error()
{
  return (fp == 0);
}

void FileWriter::flush()
{
  if (fp)
  {
    if (buffer_count)
    {
      fwrite( buffer, 1, buffer_count, fp );
    }
    fflush( fp );
  }
  buffer_count = 0;
}

void FileWriter::write_byte( Byte value )
{
  if (buffer_count == BUFFER_SIZE)
  {
    flush();
  }
  buffer[ buffer_count++ ] = value;
}

//=============================================================================
//  JSONParser
//=============================================================================
JSONParser::JSONParser()
  : next_character_cached(false)
{
  reader = standard_input_singleton.retain();
}

JSONParser::JSONParser( const JSONParser& other )
  : next_character_cached(false)
{
  reader = other.reader->retain();
  next_character_cached = other.next_character_cached;
}

JSONParser::JSONParser( File file )
  : next_character_cached(false)
{
  reader = (new FileReader( file ))->retain();
}

JSONParser::JSONParser( Value value )
  : next_character_cached(false)
{
  reader = (new StringReader( value ))->retain();
}

JSONParser::~JSONParser()
{
  reader->release();
}


JSONParser& JSONParser::operator=( const JSONParser& other )
{
  other.reader->retain();
  reader->release();
  reader = other.reader;
  next_character_cached = other.next_character_cached;
  return *this;
}

void JSONParser::close()
{
  reader->close();
}

Logical JSONParser::consume( Character ch )
{
  if (peek_character() != ch) return false;
  read_character();
  return true;
}

Logical JSONParser::consume( const char* st )
{
  --st;
  char ch;
  while ((ch = *(++st)))
  {
    if (peek_character() != ch) return false;
    read_character();
  }
  return true;
}

void JSONParser::consume_whitespace()
{
  while (has_another())
  {
    Character ch = peek_character();
    if (ch < 32 || ch == 127) read_character();
    else                      return;
  }
}

Logical JSONParser::has_another()
{
  return next_character_cached || reader->has_another();
}

Character JSONParser::peek_character()
{
  if ( !next_character_cached )
  {
    next_character = reader->read();
    next_character_cached = true;
  }
  return next_character;
}

Character JSONParser::read_character()
{
  if (next_character_cached)
  {
    next_character_cached = false;
    return next_character;
  }
  else
  {
    return reader->read();
  }
}

Integer JSONParser::read_hex_value()
{
  Integer ch = read_character();
  if (ch >= '0' && ch <= '9') return (ch - '0');
  if (ch >= 'a' && ch <= 'f') return (ch - 'a') + 10;
  if (ch >= 'A' && ch <= 'F') return (ch - 'A') + 10;
  return 0;
}

Value JSONParser::parse_identifier()
{
  StringBuilder buffer;
  Character ch = peek_character();
  while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_')
  {
    buffer.print( read_character() );
    ch = peek_character();
  }
  return Value( buffer.to_string() );
}

Value JSONParser::parse_string( Character terminator )
{
  if ( !consume(terminator) ) return Value();

  StringBuilder buffer;
  while (has_another())
  {
    if (consume(terminator)) return Value( buffer.to_string() );

    Character ch = read_character();
    if (ch == '\\')
    {
      ch = read_character();
      switch (ch)
      {
        case '"':  buffer.print( '"' ); break;
        case '\'': buffer.print( '\'' ); break;
        case '\\': buffer.print( '\\' ); break;
        case '/':  buffer.print( '/' ); break;
        case 'b':  buffer.print( '\b' ); break;
        case 'f':  buffer.print( '\f' ); break;
        case 'n':  buffer.print( '\n' ); break;
        case 'r':  buffer.print( '\r' ); break;
        case 't':  buffer.print( '\t' ); break;

        case 'u':
        {
          // Four digit hex character
          Integer code = read_hex_value();
          code = (code << 4) | read_hex_value();
          code = (code << 4) | read_hex_value();
          code = (code << 4) | read_hex_value();
          buffer.print( (Character) code );
          break;
        }

        case 's':
        {
          // Six digit hex character
          Integer code = read_hex_value();
          code = (code << 4) | read_hex_value();
          code = (code << 4) | read_hex_value();
          code = (code << 4) | read_hex_value();
          code = (code << 4) | read_hex_value();
          code = (code << 4) | read_hex_value();

          // Surrogate pair
          code -= 0x10000;
          buffer.print( (Character)(0xd800 + ((code>>10)&0x3ff)) );
          buffer.print( (Character)(0xdc00 + (code&0x3ff)) );
          break;
        }

        default:
          // Syntax error
          return Value();
      }
    }
    else
    {
      buffer.print( ch );
    }
  }

  return Value();  // ran out of input before closing quotes
}

Value JSONParser::parse_value()
{
  consume_whitespace();
  Character ch = peek_character();
  switch (ch)
  {
    case '"':
      return parse_string( '"' );

    case '\'':
      return parse_string( '\'' );

    case '[':
    {
      Value list = List();

      consume( '[' );
      consume_whitespace();

      if (consume(']')) return list;

      Logical first = true;
      while (first || consume(','))
      {
        first = false;
        consume_whitespace();
        list.add( parse_value() );
        consume_whitespace();
      }

      if ( !consume(']') ) return Value();
      return list;
    }

    case '{':
    {
      Value table = Table();

      consume( '{' );
      consume_whitespace();

      if (consume('}')) return table;

      Logical first = true;
      while (first || consume(','))
      {
        first = false;
        consume_whitespace();
        Value key   = parse_value();
        if ( !consume(':') ) return Value();
        Value value = parse_value();
        table.set( key, value );
        consume_whitespace();
      }

      if ( !consume('}') ) return Value();

      ((TableData*)(table.data))->reorder();
      return table;
    }

    default:
      if (ch == '-' || (ch >= '0' && ch <= '9'))
      {
        // parse number
        char st[300+1];
        Integer len = 0;
        st[len++] = (char) read_character();

        ch = peek_character();
        while (ch >= '0' && ch <= '9' && len<300)
        {
          st[len++] = (char) read_character();
          ch = peek_character();
        }

        if (consume('.') && len < 300)
        {
          // Fraction
          st[len++] = '.';
          ch = peek_character();
          while (ch >= '0' && ch <= '9' && len<300)
          {
            st[len++] = (char) read_character();
            ch = peek_character();
          }
        }

        if ((consume('e') || consume('E')) && len < 300)
        {
          // e/E
          st[len++] = 'e';

          // [+/-]
          if (consume('+') && len < 300)      st[len++] = '+';
          else if (consume('-') && len < 300) st[len++] = '-';

          // Exponent
          ch = peek_character();
          while (ch >= '0' && ch <= '9' && len<300)
          {
            st[len++] = (char) read_character();
            ch = peek_character();
          }
        }

        st[len] = 0;

        Real value;
        sscanf( st, "%lf", &value );
        return Value( value );
      }
      else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
      {
        Value id = parse_identifier();

        if (ch == 'f')
        {
          if (id == "false") return Value( false );
        }
        else if (ch == 'n')
        {
          if (id == "null") return Value();
        }
        else if (ch == 't')
        {
          if (id == "true") return Value( true );
        }

        return id;  // treated as string
      }
  }

  // Error - read a character from the input to avoid an infinite loop.
  read_character();

  return Value();
}

//=============================================================================
//  UTF8CharacterReader
//=============================================================================
Character UTF8CharacterReader::read()
{
  Character ch1 = read_byte();
  if (ch1 < 0x80) return ch1;

  if ((ch1 & 0xe0) == 0xc0)
  {
    // Encoded as two bytes.
    // 110xxxyy 10yyyyyy
    Character ch2 = read_byte();
    return ((ch1 << 6) & 0x7c0) | (ch2 & 0x3f);
  }
  else
  {
    // Encoded as three bytes.
    // xxxx xxxx yyyy yyyy -> 1110xxxx 10xxxxyy 10yyyyyy
    Character ch2 = read_byte();
    Character ch3 = read_byte();
    return ((ch1 << 12) & 0xf000) | ((ch2 << 6) & 0x0fc0) | (ch3 & 0x3f);
  }
}


//=============================================================================
//  StandardInput
//=============================================================================
StandardInput::StandardInput()
{
  reference_count = 1;
}

Byte StandardInput::read_byte()
{
  return getc( stdin );
}

StandardInput standard_input_singleton;


//=============================================================================
//  FileReader
//=============================================================================
FileReader::FileReader( File file )
  : buffer_position(0), buffer_count(0), remaining(0), fp(0)
{
  fp = file.reader();

  if (fp)
  {
    fseek( fp, 0, SEEK_END );
    remaining = (Long) ftell( fp );
    fseek( fp, 0, SEEK_SET );
  }
}

Logical FileReader::has_another()
{
  return (remaining > 0);
}

Byte FileReader::read_byte()
{
  if ( !remaining ) return 0;

  --remaining;
  if (buffer_position == buffer_count)
  {
    // Refill buffer
    buffer_count = fread( buffer, 1, BUFFER_SIZE, fp );
    buffer_position = 0;
  }

  return buffer[ buffer_position++ ];
}


//=============================================================================
//  StringReader
//=============================================================================
StringReader::StringReader( Value string )
{
  string = string.to_string();
  characters = string.characters();
  count = string.count();
  position = 0;
}

Logical StringReader::has_another()
{
  return (position < count);
}

Character StringReader::read()
{
  if (position == count) return (Character) 0;
  return characters[ position++ ];
}
//=============================================================================
//  StringBuilder
//=============================================================================
void StringBuilderWriter::write( Character code ) { builder->print(code); }

StringBuilder::StringBuilder( Integer initial_capacity )
    : characters(internal_buffer), capacity(STRING_BUILDER_INTERNAL_CAPACITY), 
      count(0)
{
  reserve( initial_capacity );
  writer.init( this );
}

StringBuilder::~StringBuilder()
{
  if (characters != internal_buffer) delete characters;
}

StringBuilder* StringBuilder::clear()
{
  count = 0;
  return this;
}

StringBuilder* StringBuilder::display()
{
  Integer count = this->count;
  for (int i=0; i<count; ++i)
  {
    stdout_singleton.write( characters[i] );
  }
  return this;
}

StringBuilder* StringBuilder::print( const char* st )
{
  reserve( strlen(st) );
  --st;
  char ch;
  while ((ch = *(++st)))
  {
    characters[ count++ ] = (Character) ch;
  }
  return this;
}

StringBuilder* StringBuilder::print( Character value )
{
  reserve( 1 );
  characters[ count++ ] = value;
  return this;
}

StringBuilder* StringBuilder::print( String* value )
{
  Integer other_count = value->length;
  reserve( other_count );
  memcpy( characters+count, value->character_data, other_count*sizeof(Character) );
  count += other_count;
  return this;
}

StringBuilder* StringBuilder::print( Value value )
{
  switch (value.data->type())
  {
    case CARGO_NULL:   return print( "null" );
    case CARGO_FALSE:  return print( "false" );
    case CARGO_TRUE:   return print( "true" );

    case CARGO_NUMBER:
    {
      char st[80];
      Real num = value.to_real();

      if (num == (Integer)num) sprintf( st, "%d", (Integer)num );
      else                     sprintf( st, "%lf", num );

      return print( st );
    }

    case CARGO_STRING:
      return print( (String*)(value.data) );

    default:
      return print( "(unhandled type() in StringBuilder::print())" );
  }
}

StringBuilder* StringBuilder::println()
{
  return print( (Character) 10 );
}

StringBuilder* StringBuilder::println( const char* value )
{
  return print( value )->print( (Character) 10 );
}

StringBuilder* StringBuilder::println( Character value )
{
  return print( value )->print( (Character) 10 );
}

StringBuilder* StringBuilder::println( String* value )
{
  return print( value )->print( (Character) 10 );
}

StringBuilder* StringBuilder::println( Value value )
{
  return print( value )->print( (Character) 10 );
}

StringBuilder* StringBuilder::reserve( Integer additional_capacity )
{
  Integer required_capacity = count + additional_capacity;
  if (capacity >= required_capacity) return this;

  Integer double_capacity = capacity * 2;
  if (double_capacity > required_capacity) required_capacity = double_capacity;

  Character* new_characters = new Character[ required_capacity ];
  memcpy( new_characters, characters, count*sizeof(Character) );

  if (characters != internal_buffer) delete characters;
  characters = new_characters;
  capacity = required_capacity;

  return this;
}

Data* StringBuilder::to_string()
{
  return new String(characters,count);
}

void StringBuilder::write_character( Character unicode )
{
  print( unicode );
}

}; // namespace Cargo

