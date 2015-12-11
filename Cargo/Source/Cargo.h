//=============================================================================
//  Cargo.h
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
#pragma once

#if defined(_WIN32)
#  include <windows.h>
#else
#  include <cstdint>
#endif

// For FILE
#include <cstdio>

namespace Cargo
{

#if defined(_WIN32)
  typedef double           Real;
  typedef long long int    Long;
  typedef __int32          Integer;
  typedef unsigned short   Character;
  typedef unsigned char    Byte;
  typedef bool             Logical;
#else
  typedef double           Real;
  typedef int64_t          Long;
  typedef int32_t          Integer;
  typedef uint16_t         Character;
  typedef uint8_t          Byte;
  typedef bool             Logical;
#endif

enum Type
{ 
  CARGO_NULL, CARGO_FALSE, CARGO_TRUE, CARGO_NUMBER, CARGO_STRING, CARGO_LIST, CARGO_TABLE, CARGO_NATIVE_DATA
};

typedef void (*NativeDataReleaseFn)( void* data );

// FORWARD REFERENCES
struct Data;
struct StringBuilder;
struct TableEntry;

//=============================================================================
//  Value
//=============================================================================
struct Value
{
  // PROPERTIES
  Data* data;

  // METHODS
  Value();
  Value( const Value& other );
  Value( const Value* other );
  Value( Data* data );

  Value( char value );
  Value( const char* value );
  Value( Character value );
  Value( Character* value, Integer character_count );
  Value( Logical value );
  Value( Integer value );
  Value( Long value );
  Value( Real value );
  Value( void* value, NativeDataReleaseFn release_fn=0 );
  ~Value();

  Value&     add( Value value );
  Value&     add_all( Value list );
  Value      at( Integer index ) const;
  Character  character_at( Integer index ) const;
  Character* characters() const;
  Value&     clear();
  Value&     clone();
  Logical    contains( Value value ) const;
  Integer    count() const;
  Value      first() const { return at(0); }
  Value      from( Integer i1 );
  Value      from( Integer i1, Integer i2 );
  Value      get( Integer index ) const;
  Value      get( Value key ) const;
  Integer    get_integer( Value key, Integer default_value=0 ) const;
  Value      get_list( Value key ) const;
  Logical    get_logical( Value key, Logical default_value=false ) const;
  Real       get_real( Value key, Real default_value=0.0 ) const;
  Value      get_string( Value key, Value default_value="" ) const;
  Value      get_table( Value key ) const;
  Integer    hash_code();
  Value&     insert( Value value );

  Logical    is_null()    const;
  Logical    is_logical() const;
  Logical    is_integer() const;
  Logical    is_number()  const;
  Logical    is_string()  const;
  Logical    is_list()    const;
  Logical    is_table()   const;
  Logical    is_native_data() const;

  Value      last() const { return at( count() - 1 ); }
  Integer    locate( Value value, Integer i1=0 ) const;
  Integer    locate_last( Value value ) const;
  Integer    locate_last( Value value, Integer i1 ) const;
  void*      native_data() const;

  Value      operator[]( Integer index ) const { return get(index); }

  Value      operator+( const Value& other ) const;
  Value      operator-( const Value& other ) const;
  Value      operator*( const Value& other ) const;
  Value      operator/( const Value& other ) const;
  Value      operator%( const Value& other ) const;

  Value&     operator+=( const Value& other );
  Value&     operator-=( const Value& other );
  Value&     operator*=( const Value& other );
  Value&     operator/=( const Value& other );
  Value&     operator%=( const Value& other );

  Value&     operator=( const Value& other );
  Logical    operator==( const Value& other ) const;
  Logical    operator!=( const Value& other ) const;
  Logical    operator<=( const Value& other ) const;
  Logical    operator>=( const Value& other ) const;
  Logical    operator<( const Value& other ) const;
  Logical    operator>( const Value& other ) const;

  void       print() const;
  void       println() const;

  Value      remove( Value key );
  Value      remove_at( Integer index );
  Value      remove_first() { return remove_at(0); }
  Value      remove_last() { return remove_at(count()-1); }

  Value&     set( Value key, Value value );

  char*      to_c_string( char* buffer=0, Integer buffer_size=0 ) const;
  Integer    to_integer() const;
  Value      to_json() const;
  Logical    to_logical() const;
  Real       to_real() const;
  Value      to_string() const;
};

//=============================================================================
//  Data
//  Base class for all the different data types that a Cargo Value can have.
//=============================================================================
struct Data
{
  int reference_count;

  Data();
  virtual ~Data();

  virtual void       add( Data* value ) { }
  virtual Data*      at( Integer index ) { return get(index); }
  virtual Character  character_at( Integer index ) { return 0; }
  virtual Character* characters() { return 0; }
  virtual void       clear() { }
  virtual Data*      clone() { return this; }
  virtual Integer    compare_to( Data* other );
  virtual Logical    contains( Value key ) { return locate(key) != -1; }
  virtual Integer    count() { return 0; }

  virtual TableEntry* find( Value key ) { return 0; }
  virtual Data*   from( Integer i1, Integer i2 );
  virtual Data*   get( Integer index );
  virtual Data*   get( Value key );
  virtual Integer hash_code() { return (Integer)(intptr_t)this; } 
  virtual void    insert( Data* value ) { }
  virtual Integer locate( Value value, Integer i1=0 ) { return -1; }
  virtual Integer locate_last( Value value, Integer i2 ) { return -1; }
  virtual void*   native_data() { return 0; }

  virtual Value   remove( Value key );
  virtual Value   remove_at( Integer index );

  Data* retain();
  Data* release();
  virtual void set( Value key, Value value ) { }
  virtual Type type() = 0;

  virtual Integer to_integer() = 0;
  virtual Data*   to_json();
  virtual Logical to_logical() = 0;
  virtual Real    to_real() = 0;
  virtual Data*   to_string();
};


//=============================================================================
//  NullData
//=============================================================================
struct NullData : Data
{
  NullData() { reference_count = 1; }

  Integer hash_code() { return 0; }

  Integer to_integer() { return 0; }
  Data*   to_json();
  Logical to_logical() { return false; }
  Real    to_real() { return 0; }
  Data*   to_string();

  Type    type() { return CARGO_NULL; }
};

extern NullData null_singleton;


//=============================================================================
//  False
//=============================================================================
struct False : Data
{
  False() { reference_count = 1; }

  Integer hash_code() { return 0; }

  Integer to_integer() { return 0; }
  Data*   to_json();
  Logical to_logical() { return false; }
  Real    to_real() { return 0; }
  Data*   to_string();

  Type type() { return CARGO_FALSE; }
};

extern False false_singleton;


//=============================================================================
//  True
//=============================================================================
struct True : Data
{
  True() { reference_count = 1; }

  Integer hash_code() { return 1; }

  Integer to_integer() { return 1; }
  Data*   to_json();
  Logical to_logical() { return true; }
  Real    to_real() { return 1; }
  Data*   to_string();

  Type type() { return CARGO_TRUE; }
};

extern True true_singleton;


//=============================================================================
//  Number
//=============================================================================
struct Number : Data
{
  // PROPERTIES
  Real value;

  // METHODS
  Number( Real value );

  Integer compare_to( Data* other );

  Integer hash_code();

  Integer to_integer() { return (Integer) value; }
  Logical to_logical() { return value != 0; }
  Real    to_real() { return value; }

  Type    type() { return CARGO_NUMBER; }

  // ROUTINES
  static void to_c_string( Real value, char* buffer, int size );
};


//=============================================================================
//  String
//=============================================================================
struct String : Data
{
  Character* character_data;
  Integer    length;
  Integer    hash;
  Logical    hash_computed;

  String( const char* utf8, Integer len=-1 );
  String( Character* data, Integer len );
  String( Integer length );
  ~String();

  Character  character_at( Integer index );
  Character* characters() { return this->character_data; }
  Integer    compare_to( const char* other );
  Integer    compare_to( Data* other );
  Integer    count() { return length; }

  Data*      from( Integer i1, Integer i2 );
  Data*      get( Integer index );
  Integer    hash_code();
  Integer    locate( Value value, Integer i1 );
  Integer    locate_last( Value value, Integer i1 );

  char*      to_utf8_string( char* buffer=0, Integer buffer_size=0 );
  Integer    to_integer();
  Logical    to_logical();
  Real       to_real();
  Data*      to_string();

  Type       type() { return CARGO_STRING; }

  Integer utf8_count();
};


struct GlobalString : String
{
  GlobalString( const char* utf8 ) : String(utf8) { reference_count = 1; }
};

extern GlobalString empty_string_singleton;


extern Value PI;
extern Value Null;

Value operator+( Integer lhs, Value rhs );
Value operator+( Real lhs, Value rhs );
Value operator+( const char* lhs, Value rhs );
Value operator+( Logical lhs, Value rhs );
Value operator-( Integer lhs, Value rhs );
Value operator-( Real lhs, Value rhs );
Value operator-( Logical lhs, Value rhs );
Value operator*( Integer lhs, Value rhs );
Value operator*( Real lhs, Value rhs );
Value operator*( Logical lhs, Value rhs );
Value operator/( Integer lhs, Value rhs );
Value operator/( Real lhs, Value rhs );
Value operator/( Logical lhs, Value rhs );
Value operator%( Integer lhs, Value rhs );
Value operator%( Real lhs, Value rhs );
Value operator%( Logical lhs, Value rhs );

// List and Table creators
Value JSON( const char* json );
Value JSON( Character* json, Integer count );
Value List( Integer initial_capacity=10 );
Value Table( Integer initial_capacity=16 );

//=============================================================================
//  ListData
//=============================================================================
struct ListData : Data
{
  Value*  data;
  Integer capacity;
  Integer element_count;

  ListData( Integer initial_capacity=10 );
  ~ListData();

  void    add( Data* value );
  void    clear();
  Data*   clone();
  Logical contains( Value key );
  Integer count() { return element_count; }

  Data*   from( Integer i1, Integer i2 );
  Data*   get( Integer index );
  void    insert( Data* value );
  Integer locate( Value value, Integer i1=0 );
  Integer locate_last( Value value, Integer i2 );

  Value   remove( Value key );
  Value   remove_at( Integer index );

  void    reserve( Integer additional_capacity );
  void    set( Value key, Value value );
  Integer to_integer();
  Logical to_logical();
  Real    to_real();

  Type    type() { return CARGO_LIST; }
};


//=============================================================================
//  TableEntry
//=============================================================================
struct TableEntry
{
  Value       key;
  Value       value;
  Integer     hash_code;
  TableEntry* next_entry;

  TableEntry( Value key, Value value, Integer hash_code, TableEntry* next_entry );
};


//=============================================================================
//  TableData
//=============================================================================
struct TableData : Data
{
  TableEntry** bins;
  Integer      bin_count;
  Integer      bin_mask;
  Integer      element_count;
  Integer      capacity;
  // Any additional properties must be added to the resizing code of set().

  TableData( Integer initial_capacity=8 );
  ~TableData();

  Data*   at( Integer index );
  void    clear();
  Data*   clone();
  Integer count() { return element_count; }
  Logical contains( Value value ) { return !!find(value); }

  TableEntry* find( Value key );

  Data*   get( Integer index );
  Data*   get( Value key );

  Value   remove( Value key );
  Value   remove_at( Integer index );
  Value   remove_entry( TableEntry* entry );

  void    reorder();

  void    set( Value key, Value value );

  Integer to_integer();
  Logical to_logical();
  Real    to_real();

  Type    type() { return CARGO_TABLE; }
};

//=============================================================================
//  NativeData
//=============================================================================
void FreeOnRelease( void* data );

struct NativeData : Data
{
  // PROPERTIES
  void*               data;
  NativeDataReleaseFn release_fn;

  // METHODS
  NativeData( void* data, NativeDataReleaseFn release_fn=0 );
  ~NativeData();

  Integer compare_to( Data* other );

  Integer hash_code() { return (Integer)(intptr_t) data; }
  void*   native_data() { return data; }

  Integer to_integer() { return (Integer)(intptr_t) data; }
  Logical to_logical() { return data != 0; }
  Real    to_real() { return (Real)(intptr_t) data; }

  Type    type() { return CARGO_NATIVE_DATA; }
};


//=============================================================================
//  File
//=============================================================================
struct File
{
  Value filepath;

  File( Value filepath );
  File( Value path, Value name );
  File( const File& other );
  File& operator=( const File& other );

  Value   absolute_filepath();
  Value   base_filename();
  Logical exists();
  Value   extension();
  Value   filename();
  Logical is_folder();

  Value   listing();
  Value   load_json();
  Value   load_list();
  Value   load_table();

  Value   path();

  FILE*   reader();

  bool    save( Value value, Logical force_ascii=false );
  Long    size();

  FILE*   writer();

  static Value   absolute_filepath( Value filepath );
  static Value   base_filename( Value filepath );
  static Logical exists( Value filepath );
  static Value   extension( Value filepath );
  static Value   filename( Value filepath );
  static Value   home_folder();
  static Logical is_folder( Value filepath );
  static Value   listing( Value filepath );
  static Value   load_json( Value filepath );
  static Value   load_list( Value filepath );
  static Value   load_table( Value filepath );
  static Value   path( Value filepath );
  static bool    save( Value filepath, Value value, Logical force_ascii=false );
  static Long    size( Value filepath );
};


//=============================================================================
//  Printer
//=============================================================================
struct CharacterReader;
struct CharacterWriter;

struct Printer
{
  CharacterWriter* writer;
  Logical          force_ascii;
  Logical          at_newline;
  Integer          indent;

  Printer();
  Printer( const Printer& other );
  Printer( CharacterWriter* writer ) : writer(writer) {}
  Printer( File file, Logical force_ascii=false );
  Printer( StringBuilder* builder );
  ~Printer();

  Printer& operator=( const Printer& other );

  void    close();
  Logical error();
  void    flush();
  void    print( const char* c_string );
  void    print( Character code );
  void    print( Value value, Logical quote_strings=false );
  void    println() { print('\n'); }
  void    println( Character code ) { print(code); println(); }
};


//=============================================================================
//  JSONPrinter
//=============================================================================
struct JSONPrinter
{
  CharacterWriter* writer;
  Logical          force_ascii;

  JSONPrinter();
  JSONPrinter( const JSONPrinter& other );
  JSONPrinter( CharacterWriter* writer ) : writer(writer) {}
  JSONPrinter( File file, Logical force_ascii=false );
  JSONPrinter( StringBuilder* builder );
  ~JSONPrinter();

  JSONPrinter& operator=( const JSONPrinter& other );

  void    close();
  Logical error();
  void    flush();
  void    print( const char* c_string );
  void    print( Character code );
  void    print( Value value );
};


//=============================================================================
//  CharacterWriter
//=============================================================================
struct CharacterWriter
{
  // PROPERTIES
  Integer reference_count;

  // METHODS
  virtual ~CharacterWriter() { }

  CharacterWriter* retain() { ++reference_count; return this; }
  void             release() { if ( !(--reference_count) ) delete this; }

  virtual void    close() { }
  virtual Logical error() { return false; }
  virtual void    flush() { }
  virtual void    write( Character code ) = 0;
};


//=============================================================================
//  UTF8Writer
//=============================================================================
struct UTF8Writer : CharacterWriter
{
  void write( Character code );
  virtual void write_byte( Byte b ) = 0;
};


//=============================================================================
//  StandardOutput
//=============================================================================
struct StandardOutput : UTF8Writer
{
  StandardOutput();
  void write_byte( Byte b );
};

extern StandardOutput stdout_singleton;


//=============================================================================
//  FileWriter
//=============================================================================
struct FileWriter : UTF8Writer
{
  // SETTINGS
  static const Integer BUFFER_SIZE = 4096;

  // PROPERTIES
  Byte    buffer[BUFFER_SIZE];
  Integer buffer_count;
  FILE*   fp;

  // METHODS
  FileWriter( File file );
  ~FileWriter();

  void    close();
  Logical error();
  void    flush();
  void    write_byte( Byte b );
};


//=============================================================================
//  JSONParser
//=============================================================================
struct JSONParser
{
  // PROPERTIES
  CharacterReader* reader;
  Character        next_character;
  Logical          next_character_cached;

  // METHODS
  JSONParser();
  JSONParser( const JSONParser& other );
  JSONParser( CharacterReader* reader ) : reader(reader) {}
  JSONParser( File file );
  JSONParser( Value value );
  ~JSONParser();

  JSONParser& operator=( const JSONParser& other );

  void      close();
  Logical   consume( Character ch );
  Logical   consume( const char* st );
  void      consume_whitespace();
  Logical   has_another();
  Character peek_character();
  Character read_character();
  Integer   read_hex_value();
  Value     parse_identifier();
  Value     parse_string( Character terminator='"' );
  Value     parse_value();
};


//=============================================================================
//  CharacterReader
//=============================================================================
struct CharacterReader
{
  // PROPERTIES
  Integer reference_count;

  // METHODS
  virtual ~CharacterReader() { }

  CharacterReader* retain() { ++reference_count; return this; }
  void             release() { if ( !(--reference_count) ) delete this; }

  virtual void      close() { }
  virtual Logical   has_another() = 0;
  virtual Character read() = 0;
};


//=============================================================================
//  UTF8CharacterReader
//=============================================================================
struct UTF8CharacterReader : CharacterReader
{
  // METHODS
  virtual Character read();
  virtual Byte      read_byte() = 0;
};


//=============================================================================
//  StandardInput
//=============================================================================
struct StandardInput : UTF8CharacterReader
{
  // METHODS
  StandardInput();

  Logical   has_another() { return true; }
  Byte      read_byte();
};

extern StandardInput standard_input_singleton;

//=============================================================================
//  FileReader
//=============================================================================
struct FileReader : UTF8CharacterReader
{
  // SETTINGS
  static const Integer BUFFER_SIZE = 4096;

  // PROPERTIES
  char    buffer[ BUFFER_SIZE ];
  Integer buffer_position;
  Integer buffer_count;
  Long    remaining;
  FILE*   fp;

  // METHODS
  FileReader( File file );
  Logical   has_another();
  Byte      read_byte();
};


//=============================================================================
//  StringReader
//=============================================================================
struct StringReader : CharacterReader
{
  // PROPERTIES
  Value      string;  // to retain
  Character* characters;
  Integer    count;
  Integer    position;

  // METHODS
  StringReader( Value string );

  Logical   has_another();
  Character read();
};



//=============================================================================
//  StringBuilder
//=============================================================================
enum { STRING_BUILDER_INTERNAL_CAPACITY=128 };

struct StringBuilderWriter : CharacterWriter
{
  // This is embedded in each StringBuilder and should not be dynamically
  // allocated.

  // PROPERTIES
  StringBuilder* builder;

  // METHODS
  StringBuilderWriter() { reference_count = 1; }
  void init( StringBuilder* builder ) { this->builder = builder; }
  void write( Character code );
};


struct StringBuilder
{
  // PROPERTIES
  Character  internal_buffer[STRING_BUILDER_INTERNAL_CAPACITY];
  Character* characters;
  Integer    capacity;
  Integer    count;
  StringBuilderWriter writer;


  // METHODS
  StringBuilder( Integer initial_capacity=STRING_BUILDER_INTERNAL_CAPACITY );
  ~StringBuilder();

  StringBuilder* clear();
  StringBuilder* display();  // print to stdout

  StringBuilder* print( const char* value );
  StringBuilder* print( Character value );
  StringBuilder* print( String* value );
  StringBuilder* print( Value value );
  StringBuilder* println();
  StringBuilder* println( const char* value );
  StringBuilder* println( Character value );
  StringBuilder* println( String* value );
  StringBuilder* println( Value value );

  StringBuilder* reserve( Integer additional_capacity );

  Data*          to_string();

  void           write_character( Character unicode );
};

}; // namespace Cargo

