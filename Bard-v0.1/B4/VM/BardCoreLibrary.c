#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Bard.h"

void BardCoreLibrary_configure( BardVM* vm )
{
  BardVM_register_native_method( vm, "Console", "at_newline()", BardConsole__at_newline );
  BardVM_register_native_method( vm, "Console", "write(Character)", BardConsole__write__Character );
  BardVM_register_native_method( vm, "Console", "write(String)", BardConsole__write__String );

  BardVM_register_native_method( vm, "GenericArray", "clear_references(Integer,Integer)", BardGenericArray__clear_references__Integer_Integer );
  BardVM_register_native_method( vm, "GenericArray", "shift(Integer,Integer,Integer)", BardGenericArray__shift__Integer_Integer_Integer );

  BardVM_register_native_method( vm, "Integer64", "create(Integer,Integer)", BardInteger64__create__Integer_Integer );
  BardVM_register_native_method( vm, "Integer64", "high()", BardInteger64__high );
  BardVM_register_native_method( vm, "Integer64", "low()", BardInteger64__low );
  BardVM_register_native_method( vm, "Integer64", "operator+(Integer64,Integer64)", BardInteger64__operatorPLUS__Integer64_Integer64 );
  BardVM_register_native_method( vm, "Integer64", "operator-(Integer64,Integer64)", BardInteger64__operatorMINUS__Integer64_Integer64 );
  BardVM_register_native_method( vm, "Integer64", "operator*(Integer64,Integer64)", BardInteger64__operatorTIMES__Integer64_Integer64 );
  BardVM_register_native_method( vm, "Integer64", "operator/(Integer64,Integer64)", BardInteger64__operatorDIVIDEDBY__Integer64_Integer64 );
  BardVM_register_native_method( vm, "Integer64", "to_Real()", BardInteger64__to_Real );

  BardVM_register_native_method( vm, "Math", "floor(Real)", BardMath__floor__Real );
  BardVM_register_native_method( vm, "Math", "sqrt(Real)", BardMath__sqrt__Real );

  BardVM_register_native_method( vm, "NativeFileReader", "create()", BardNativeFileReader__create );
  BardVM_register_native_method( vm, "NativeFileReader", "create(String)", BardNativeFileReader__create__String );
  BardVM_register_native_method( vm, "NativeFileReader", "available()", BardNativeFileReader__available );
  BardVM_register_native_method( vm, "NativeFileReader", "close()", BardNativeFileReader__close );
  BardVM_register_native_method( vm, "NativeFileReader", "position()", BardNativeFileReader__position );
  BardVM_register_native_method( vm, "NativeFileReader", "read(Byte[],Integer)",
      BardNativeFileReader__read__ByteList_Integer );

  BardVM_register_native_method( vm, "NativeFileWriter", "create()", BardNativeFileWriter__create );
  BardVM_register_native_method( vm, "NativeFileWriter", "create(String,Logical)", BardNativeFileWriter__create__String_Logical );
  BardVM_register_native_method( vm, "NativeFileWriter", "close()", BardNativeFileWriter__close );
  BardVM_register_native_method( vm, "NativeFileWriter", "position()", BardNativeFileWriter__position );
  BardVM_register_native_method( vm, "NativeFileWriter", "write(Byte[],Integer,Integer)",
      BardNativeFileWriter__write__ByteList_Integer_Integer );

  BardVM_register_native_method( vm, "Object", "create_instance()", BardObject__create_instance );

  BardVM_register_native_method( vm, "Random", "next_state(Real)", BardRandom__next_state__Real );

  BardVM_register_native_method( vm, "Runtime", "describe(Integer)", BardRuntime__describe__Integer );
  BardVM_register_native_method( vm, "Runtime", "ip(Integer)", BardRuntime__ip__Integer );

  BardVM_register_native_method( vm, "String", "create()", BardString__create );
  BardVM_register_native_method( vm, "String", "create(Byte[])", BardString__create__ByteList );
  BardVM_register_native_method( vm, "String", "create(Character[])", BardString__create__CharacterList );
  BardVM_register_native_method( vm, "String", "count()", BardString__count );
  BardVM_register_native_method( vm, "String", "get(Integer)", BardString__get__Integer );
  BardVM_register_native_method( vm, "String", "hash_code()", BardString__hash_code );
  //BardVM_register_native_method( vm, "String", "compare_to(String)", BardString__compare_to__String );
  //BardVM_register_native_method( vm, "String", "equals(String)", BardString__equals__String );
  //BardVM_register_native_method( vm, "String", "trace_references()", BardString__trace_references );

  //BardVM_register_native_method( vm, "StringBuilder", "write(String)", BardStringBuilder__write__String );

  BardVM_register_native_method( vm, "Time", "current()", BardTime__current );
}

//=============================================================================
//  Array
//=============================================================================


//=============================================================================
//  Console
//=============================================================================
BardCharacter BardConsole_last_character_printed = 10;

void BardConsole__at_newline( BardVM* vm )
{
  vm->sp->integer = (BardConsole_last_character_printed == 10);
}

void BardConsole__write__Character( BardVM* vm )
{
  BardCharacter ch = BardVM_pop_character( vm );
  putc( (char) ch, stdout );
  BardConsole_last_character_printed = ch;
  fflush( stdout );
}


void BardConsole__write__String( BardVM* vm )
{
  BardString* st = BardVM_pop_string( vm );

  if (st)
  {
    int count = st->count;
    if (count)
    {
      BardCharacter* data = st->characters - 1;

      while (--count >= 0) putc( (char)*(++data), stdout );
      BardConsole_last_character_printed = *data;
    }
    fflush( stdout );
  }
  else
  {
    printf( "null" );
    BardConsole_last_character_printed = 'l';
    fflush( stdout );
  }
}


//=============================================================================
//  GenericArray
//=============================================================================
void BardGenericArray__clear_references__Integer_Integer( BardVM* vm )
{
  int i2 = BardVM_pop_integer( vm );
  int i1 = BardVM_pop_integer( vm );
  BardArray* array = (BardArray*) BardVM_pop_object( vm );
  int count = array->count;

  if ( !array->is_reference_array ) return;

  ++i2;
  if (i1 < 0) i1 = 0;
  if (i2 > count) i2 = count;
  if (i1 >= i2) return;

  bzero( array->object_data + i1, (i2-i1)*sizeof(BardObject*) );
}

void BardGenericArray__shift__Integer_Integer_Integer( BardVM* vm )
{
  int delta = BardVM_pop_integer( vm );
  int i2 = BardVM_pop_integer( vm ) + 1;
  int i1 = BardVM_pop_integer( vm );
  BardArray* array = (BardArray*) BardVM_pop_object( vm );
  int count = array->count;
  int element_size = array->element_size;

  int src_i;
  if (i1 < 0) i1 = 0;
  src_i = i1;

  i1 += delta;
  i2 += delta;

  if (i1 < 0)
  {
    src_i += -i1;
    i1 = 0;
  }

  if (i2 > count) i2 = count;


  if (i1 >= i2 || src_i == i1) return;
  memmove( array->byte_data + i1*element_size, array->byte_data + src_i*element_size, (i2-i1)*element_size );
}


//=============================================================================
//  Integer64
//=============================================================================
void BardInteger64__create__Integer_Integer( BardVM* vm )
{
  BardStackValue* sp = (vm->sp += 2);  // one for 'this', one for 'low'
  sp->integer64 = ((((BardInt64)(sp[-1].integer)) << 32LL) | (((BardInt64)(sp[-2].integer)) & 0xffffFFFFLL));
}

void BardInteger64__high( BardVM* vm )
{
  vm->sp->integer = (BardInteger) (vm->sp->integer64 >> 32);
}

void BardInteger64__low( BardVM* vm )
{
  vm->sp->integer = (BardInteger) vm->sp->integer64;
}

void BardInteger64__operatorPLUS__Integer64_Integer64( BardVM* vm )
{
  ++vm->sp;
  vm->sp->integer64 += vm->sp[-1].integer64;
}

void BardInteger64__operatorMINUS__Integer64_Integer64( BardVM* vm )
{
  ++vm->sp;
  vm->sp->integer64 -= vm->sp[-1].integer64;
}

void BardInteger64__operatorTIMES__Integer64_Integer64( BardVM* vm )
{
  ++vm->sp;
  vm->sp->integer64 *= vm->sp[-1].integer64;
}

void BardInteger64__operatorDIVIDEDBY__Integer64_Integer64( BardVM* vm )
{
  ++vm->sp;
  vm->sp->integer64 -= vm->sp[-1].integer64;
}

void BardInteger64__to_Real( BardVM* vm )
{
  vm->sp->real = (BardReal) vm->sp->integer64;
}


//=============================================================================
//  Math
//=============================================================================
void BardMath__floor__Real( BardVM* vm )
{
  BardReal value = BardVM_pop_real( vm );
  vm->sp->real = floor( value );
}

void BardMath__sqrt__Real( BardVM* vm )
{
  BardReal value = BardVM_pop_real( vm );
  vm->sp->real = sqrt( value );
}


//=============================================================================
//  NativeFileReader
//=============================================================================
typedef struct BardNativeFileReader
{
  BardObject header;

  FILE* fp;
  int   position;
  int   available;
} BardNativeFileReader;

void BardNativeFileReader__create( BardVM* vm )
{
  BardType* type_NativeFileReader = BardVM_find_type( vm, "NativeFileReader" );
  BardNativeFileReader* reader = (BardNativeFileReader*) BardType_create_object_of_size( 
      type_NativeFileReader, (int) sizeof(BardNativeFileReader) );
  vm->sp->object = (BardObject*) reader;
}

void BardNativeFileReader__create__String( BardVM* vm )
{
  BardString* filepath = BardVM_pop_string( vm );
  BardType* type_NativeFileReader = vm->sp->object->type;
  BardNativeFileReader* reader = (BardNativeFileReader*) BardType_create_object_of_size( 
      type_NativeFileReader, (int) sizeof(BardNativeFileReader) );

  {
    char filepath_buffer[512];
    BardString_to_c_string( filepath, filepath_buffer, 512 );
    reader->fp = fopen( filepath_buffer, "rb" );

    if (reader->fp)
    {
      fseek( reader->fp, 0, SEEK_END );
      reader->available = ftell( reader->fp );
      fseek( reader->fp, 0, SEEK_SET );
      vm->sp->object = (BardObject*) reader;
    }
    else
    {
      vm->sp->object = NULL;
    }
  }
}

void BardNativeFileReader__available( BardVM* vm )
{
  BardNativeFileReader* reader = (BardNativeFileReader*) vm->sp->object;
  vm->sp->integer = reader->available;
}

void BardNativeFileReader__close( BardVM* vm )
{
  BardNativeFileReader* reader = (BardNativeFileReader*) BardVM_pop_object( vm );
  if (reader->fp)
  {
    fclose( reader->fp );
    reader->fp = NULL;
    reader->available = 0;
  }
}

void BardNativeFileReader__position( BardVM* vm )
{
  BardNativeFileReader* reader = (BardNativeFileReader*) vm->sp->object;
  vm->sp->integer = reader->position;
}

void BardNativeFileReader__read__ByteList_Integer( BardVM* vm )
{
  int       buffer_count;
  BardByte* byte_data;

  int read_count  = BardVM_pop_integer( vm );
  BardObject* buffer = BardVM_pop_object( vm );
  BardNativeFileReader* reader = (BardNativeFileReader*) vm->sp->object;

  if (reader->fp == NULL || !reader->available)
  {
    if (reader->fp)
    {
      fclose( reader->fp );
      reader->fp = 0;
    }
    vm->sp->integer = -1;
    return;
  }

  if (read_count == -1 || read_count > reader->available) read_count = reader->available;

  buffer_count = BardList_count( buffer );
  BardList_ensure_capacity( buffer, buffer_count + read_count );

  byte_data = BardList_data( buffer )->byte_data + buffer_count;
  fread( byte_data, 1, read_count, reader->fp );

  vm->sp->integer = read_count;
  BardList_count( buffer ) = buffer_count + read_count;
  reader->available -= read_count;
  reader->position += read_count;

  if (reader->available == 0)
  {
    fclose( reader->fp );
    reader->fp = 0;
  }
}


//=============================================================================
//  NativeFileWriter
//=============================================================================
typedef struct BardNativeFileWriter
{
  BardObject header;

  FILE* fp;
} BardNativeFileWriter;

void BardNativeFileWriter__create( BardVM* vm )
{
  BardType* type_NativeFileWriter = BardVM_find_type( vm, "NativeFileWriter" );
  BardNativeFileWriter* writer = (BardNativeFileWriter*) BardType_create_object_of_size( 
      type_NativeFileWriter, (int) sizeof(BardNativeFileWriter) );
  vm->sp->object = (BardObject*) writer;
}

void BardNativeFileWriter__create__String_Logical( BardVM* vm )
{
  int append = BardVM_pop_logical( vm );
  BardString* filepath = BardVM_pop_string( vm );
  BardType* type_NativeFileWriter = vm->sp->object->type;
  BardNativeFileWriter* writer = (BardNativeFileWriter*) BardType_create_object_of_size( 
      type_NativeFileWriter, (int) sizeof(BardNativeFileWriter) );

  {
    char filepath_buffer[512];
    const char* write_mode = "wb";
    if (append) write_mode = "ab";
    BardString_to_c_string( filepath, filepath_buffer, 512 );
    writer->fp = fopen( filepath_buffer, write_mode );

    if (writer->fp)
    {
      vm->sp->object = (BardObject*) writer;
      fseek( writer->fp, 0, SEEK_END );
    }
    else
    {
      vm->sp->object = NULL;
    }
  }
}

void BardNativeFileWriter__close( BardVM* vm )
{
  BardNativeFileWriter* writer = (BardNativeFileWriter*) BardVM_pop_object( vm );
  if (writer->fp)
  {
    fclose( writer->fp );
    writer->fp = NULL;
  }
}

void BardNativeFileWriter__position( BardVM* vm )
{
  BardNativeFileWriter* writer = (BardNativeFileWriter*) vm->sp->object;
  if (writer->fp) vm->sp->integer = ftell( writer->fp );
  else            vm->sp->integer = -1;
}

void BardNativeFileWriter__write__ByteList_Integer_Integer( BardVM* vm )
{
  int count  = BardVM_pop_integer( vm );
  int offset = BardVM_pop_integer( vm );
  BardObject* list = BardVM_pop_object( vm );
  BardNativeFileWriter* writer = (BardNativeFileWriter*) BardVM_pop_object( vm );

  if ( !writer->fp ) return;

  BardByte* src = BardList_data( list )->byte_data + offset;
  fwrite( src, 1, count, writer->fp );
}


//=============================================================================
//  Object
//=============================================================================
void BardObject__create_instance( BardVM* vm )
{
  BardObject* obj = vm->sp->object;

  if (obj)
  {
    BardType* of_type = obj->type;
    if (BardType_is_class(of_type))
    {
      // Set the processor to the start of init_defaults()
      BardMethod* m = of_type->m_init_defaults;
      vm->sp->object = BardType_create_object( of_type );

      (--vm->current_frame)->fp = vm->sp;
      vm->current_frame->m  = m;
      vm->current_frame->ip = m->ip;
    }
    else
    {
      // TODO: throw error
      vm->sp->object = NULL;
    }
  }
  // else throw error
}


//=============================================================================
//  Random
//=============================================================================
void BardRandom__next_state__Real( BardVM* vm )
{
  BardInt64 state = (BardInt64) BardVM_pop_real( vm );
  BardVM_pop_discard( vm );  // context object
  BardVM_push_real( vm, (state * 0xDEECe66dLL + 11LL) & 0xFFFFffffFFFF );
}


//=============================================================================
//  Runtime
//=============================================================================
void BardRuntime__describe__Integer( BardVM* vm )
{
  BardMethod* m;
  int ip = BardVM_pop_integer( vm );
  BardVM_pop_discard(vm);  // Runtime singleton

  m = BardVM_find_method_at_ip( vm, ip );
  if (m)
  {
    char* filename;
    int   line;
    if (BardVM_find_line_info_at_ip(vm, m, ip, &filename, &line))
    {
      BardVM_push_integer( vm, line );
      BardVM_push_c_string( vm, filename );
    }
    else
    {
      BardVM_push_integer( vm, 0 );
      BardVM_push_c_string( vm, "" );
    }
    BardVM_push_c_string( vm, BardMethod_call_signature(m) );
    BardVM_push_c_string( vm, m->type_context->name );
  }
}

void BardRuntime__ip__Integer( BardVM* vm )
{
  int frame_offset = BardVM_pop_integer( vm );
  int max_offset = ((vm->call_stack + BARD_STACK_SIZE) - 3) - vm->current_frame;
  int ip;

  if (frame_offset > max_offset)
  {
    vm->sp->integer = -1;
    return;
  }

  ip = (int)((vm->current_frame[frame_offset].ip+1) - vm->code);
  vm->sp->integer = ip;
}


//=============================================================================
//  String
//=============================================================================
void BardString__create( BardVM* vm )
{
  vm->sp->object = (BardObject*) BardString_create_with_size( vm, 0 );
}

void BardString__create__ByteList( BardVM* vm )
{
  BardObject* list = BardVM_pop_object( vm );
  int count = BardList_count( list );
  BardArray* array = BardList_data(list);
  BardByte* src = array->byte_data - 1;
  BardString* st = BardString_create_with_size( vm, count );
  BardCharacter* dest = st->characters - 1;
  while (--count >= 0)
  {
    *(++dest) = *(++src);
  }
  vm->sp->object = (BardObject*) BardString_set_hash_code( st );
}

void BardString__create__CharacterList( BardVM* vm )
{
  BardObject* list = BardVM_pop_object( vm );
  int count = BardList_count( list );
  BardArray* array = BardList_data(list);
  BardString* st = BardString_create_with_size( vm, count );
  memcpy( st->characters, array->character_data, count*2 );
  vm->sp->object = (BardObject*) BardString_set_hash_code( st );
}

void BardString__count( BardVM* vm )
{
  vm->sp->integer = ((BardString*)vm->sp->object)->count;
}

void BardString__get__Integer( BardVM* vm )
{
  int index = BardVM_pop_integer( vm );
  vm->sp->integer = ((BardString*)vm->sp->object)->characters[ index ];
}

void BardString__hash_code( BardVM* vm )
{
  vm->sp->integer = ((BardString*)vm->sp->object)->hash_code;
}

/*
void BardString__compare_to__String( BardVM* vm )
{
  BardObject* b = BardVM_pop_object( vm );
  BardObject* a = BardVM_pop_object( vm );

  if (a == b)
  {
    BardVM_push_integer( vm, 0 );  // EQ
    return;
  }
  else
  {
    int a_count = BardString_count(vm,a);
    int b_count = BardString_count(vm,b);
    int count = (a_count < b_count) ? a_count : b_count;

    BardCharacter* a_data = BardString_character_array(vm,a)->character_data - 1;
    BardCharacter* b_data = BardString_character_array(vm,b)->character_data - 1;
    while (--count >=0 )
    {
      if (*(++a_data) != *(++b_data))
      {
        if (*a_data < *b_data)
        {
          BardVM_push_logical( vm, -1 );
        }
        else
        {
          BardVM_push_logical( vm, 1 );
        }
        return;
      }
    }

    if (a_count < b_count)
    {
      BardVM_push_logical( vm, -1 );  // LT
    }
    else if (a_count > b_count)
    {
      BardVM_push_logical( vm, 1 );  // GT
    }
    else
    {
      BardVM_push_logical( vm, 0 );  // EQ
    }
  }
}

void BardString__equals__String( BardVM* vm )
{
  BardObject* b = BardVM_pop_object( vm );
  BardObject* a = BardVM_pop_object( vm );
  int result = 1;

  if (a != b)
  {
    if (BardString_hash_code(vm,a) != BardString_hash_code(vm,b) || BardString_count(vm,a) != BardString_count(vm,b))
    {
      result = 0;
    }
    else
    {
      int count = BardString_count(vm,a);
      BardCharacter* a_data = BardString_character_array(vm,a)->character_data - 1;
      BardCharacter* b_data = BardString_character_array(vm,b)->character_data - 1;
      while (--count >=0 )
      {
        if (*(++a_data) != *(++b_data))
        {
          result = 0;
          break;
        }
      }
    }
  }

  BardVM_push_logical( vm, result );
}
*/


//=============================================================================
//  StringBuilder
//=============================================================================
/*
void BardStringBuilder__write__String( BardVM* vm )
{
  BardObject* st = BardVM_pop_object( vm );
  BardObject* builder = BardVM_peek_object( vm );
  BardString_write_string( builder, st );
}
*/


//=============================================================================
//  Time
//=============================================================================
void BardTime__current( BardVM* vm )
{
  vm->sp->real = BardUtil_time();
}

