//=============================================================================
//  bard_vm.cpp
//
//  2010.11.26 by Abe Pralle
//
//  $(BARD_VERSION)
//  ---------------------------------------------------------------------------
//
//  Copyright 2008-2010 Plasmaworks LLC
//
//  Licensed under the Apache License, Version 2.0 (the "License"); 
//  you may not use this file except in compliance with the License. 
//  You may obtain a copy of the License at 
//
//    http://www.apache.org/licenses/LICENSE-2.0 
//
//  Unless required by applicable law or agreed to in writing, 
//  software distributed under the License is distributed on an 
//  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
//  either express or implied. See the License for the specific 
//  language governing permissions and limitations under the License.
//=============================================================================
#include <math.h>

#include "bard.h"
#include "bard_mm.h"
#include "bard_vm.h"
#include "bard_stdlib.h"

BardVM bvm;

//=============================================================================
//  BardGenericObject
//=============================================================================
int BardGenericObject::property_offset( const char* name )
{
  return type->must_find_property(name)->offset;
}

void* BardGenericObject::property_address( const char* name )
{
  return (void*)(((char*)this) + type->must_find_property(name)->offset);
}

void* BardGenericObject::property_address( int offset )
{
  return (void*)(((char*)this) + offset);
}

void BardGenericObject::set_ref( const char* id, BardObject* object )
{
  BardObject** property = (BardObject**) (((char*)this) + type->must_find_property(id)->offset);
  if (*property) --((*property)->reference_count);
  *property = object;
  if (object) ++object->reference_count;
}

void BardGenericObject::set_ref( int offset, BardObject* object )
{
  BardObject** property = (BardObject**) (((char*)this) + offset);
  if (*property) --((*property)->reference_count);
  *property = object;
  if (object) ++object->reference_count;
}

BardObject* BardGenericObject::get_ref( const char* name )
{
  BardObject** property = (BardObject**) (((char*)this) + type->must_find_property(name)->offset);
  return *property;
}

BardObject* BardGenericObject::get_ref( int offset )
{
  BardObject** property = (BardObject**) (((char*)this) + offset);
  return *property;
}


//=============================================================================
// BardTypeInfo
//=============================================================================
bool BardTypeInfo::is_real()
{
  return (this == bvm.type_real64 || this == bvm.type_real32);
}

bool BardTypeInfo::is_integer()
{
  return (this == bvm.type_int64 || this == bvm.type_int32
    || this == bvm.type_char || this == bvm.type_byte);
}

bool BardTypeInfo::is_int32_64()
{
  return (this == bvm.type_int64 || this == bvm.type_int32);
}

bool BardTypeInfo::is_logical()
{
  return (this == bvm.type_logical);
}


BardMethodInfo* BardTypeInfo::find_method( const char* sig )
{
  for (int i=0; i<methods.count; ++i)
  {
    if (strcmp(methods[i]->signature,sig) == 0) return methods[i];
  }
  return NULL;
}

BardMethodInfo* BardTypeInfo::must_find_method( const char* sig )
{
  BardMethodInfo* m = find_method(sig);
  if ( !m ) bard_throw_fatal_error( "Can't find method ", sig, "." );
  return m;
}

BardPropertyInfo* BardTypeInfo::find_property( const char* name, BardTypeInfo* required_type )
{
  for (int i=0; i<properties.count; ++i)
  {
    BardPropertyInfo* p = properties[i];
    if (strcmp(p->name,name) == 0) 
    {
      if (required_type && p->type != required_type)
      {
        bard_throw_fatal_error( "Property '", p->name, "' is not of the required type." );
      }
      return p;
    }
  }
  return NULL;
}

BardPropertyInfo* BardTypeInfo::must_find_property( const char* name, BardTypeInfo* required_type )
{
  BardPropertyInfo* p = find_property(name,required_type);
  if ( !p ) bard_throw_fatal_error( "Can't find property ", name, "." );
  return p;
}


BardObject* BardTypeInfo::singleton()
{
  return (BardObject*) bvm.singletons[singleton_index];
}

BardObject* BardTypeInfo::create()
{
  return (BardObject*) mm.create_object(this, object_size);
}

BardArray* BardTypeInfo::create( int array_count )
{
  return mm.create_array(this,array_count);
}

//=============================================================================
// BardMethodInfo
//=============================================================================
void BardMethodInfo::create_signature()
{
  StringBuilder buffer;
  buffer.print(name);
  buffer.print("(");
  for (int i=0; i<parameters->count(); ++i)
  {
    if (i > 0) buffer.print(",");
    buffer.print( (*parameters)[i]->type->name );
  }
  buffer.print(")");
  signature = buffer.to_new_ascii();
}

//=============================================================================
//  BardString
//=============================================================================
BardTypeInfo* BardString::string_type()
{
  return bvm.type_string;
}

//=============================================================================
// Bard VM
//=============================================================================
void bard_assert( bool value, const char* st )
{
  if ( !value ) bard_throw_fatal_error( st );
}

void bard_assert( bool value, const char* st1, const char* st2, const char* st3 )
{
  if ( !value ) bard_throw_fatal_error( st1, st2, st3 );
}

void bard_throw_exception_of_type( BardTypeInfo* type )
{
  bard_throw_fatal_error( "TODO: bard_throw_exception_of_type()" );
}

//=============================================================================
//  BardVM
//=============================================================================
BardVM::~BardVM()
{
  shut_down();
}

void BardVM::shut_down()
{
  initialized = false;
  mm.shut_down();

  if (ref_stack)
  {
    delete ref_stack;
    ref_stack = NULL;
  }
  if (data_stack)
  {
    delete data_stack;
    data_stack = NULL;
  }
  if (call_stack)
  {
    delete call_stack;
    call_stack = NULL;
  }

  code.clear();
  literal_table.clear();
  address_offsets.clear();
  address_table.clear();
  line_table.clear();
  singletons.clear();
  native_method_hooks.clear();

  filenames.clear();
  identifiers.clear();
  types.clear();
  parameter_table.clear();
  methods.clear();
  strings.clear();
}

BardObject* BardVM::create_type( const char* name )
{
  BardTypeInfo* type = must_find_type(name);
  return type->create();
}

BardTypeInfo* BardVM::find_type( const char* name )
{
  int count = bvm.types.count;
  for (int i=0; i<count; ++i)
  {
    BardTypeInfo* type = bvm.types[i];
    if ( strcmp(type->name,name) == 0 ) return type;
  }
  return NULL;
}

BardTypeInfo* BardVM::must_find_type( const char* name )
{
  BardTypeInfo* result = find_type(name);
  if (result) return result;
  return NULL;
}


void BardVM::register_native_method( const char* class_name, const char* sig, BardNativeFn fn )
{
  native_method_hooks.add( BardNativeFnHook(class_name,sig,fn) );
}

static void Math__abs__Int64()
{
  BardInt64 n = BARD_POP_INTEGER();
  BARD_DISCARD_REF();
  if (n < 0) n = -n;
  BARD_PUSH_INTEGER(n);
}

static void Math__abs__Real64()
{
  BardReal64 n = BARD_POP_REAL();
  BARD_DISCARD_REF();
  if (n < 0) n = -n;
  BARD_PUSH_REAL(n);
}

static void Math__floor__Real64()
{
  BardReal64 n = BARD_POP_REAL();
  BARD_DISCARD_REF();
  BARD_PUSH_REAL(floor(n));
}

static void Math__sqrt__Real64()
{
  BardReal64 n = BARD_POP_REAL();
  BARD_DISCARD_REF();
  BARD_PUSH_REAL(sqrt(n));
}

static void Math__sin__Radians()
{
  BardReal64 rads = BARD_POP_REAL();
  BARD_DISCARD_REF();
  BARD_PUSH_REAL(sin(rads));
}

static void Math__cos__Radians()
{
  BardReal64 rads = BARD_POP_REAL();
  BARD_DISCARD_REF();
  BARD_PUSH_REAL(cos(rads));
}

static void Math__tan__Radians()
{
  BardReal64 rads = BARD_POP_REAL();
  BARD_DISCARD_REF();
  BARD_PUSH_REAL(tan(rads));
}

static void Math__asin__Real64()
{
  BardReal64 n = BARD_POP_REAL();
  BARD_DISCARD_REF();
  BARD_PUSH_REAL(asin(n));
}

static void Math__acos__Real64()
{
  BardReal64 n = BARD_POP_REAL();
  BARD_DISCARD_REF();
  BARD_PUSH_REAL(acos(n));
}

static void Math__atan__Real64()
{
  BardReal64 n = BARD_POP_REAL();
  BARD_DISCARD_REF();
  BARD_PUSH_REAL(atan(n));
}

static void Math__atan2__Real64_Real64()
{
  BardReal64 x = BARD_POP_REAL();
  BardReal64 y = BARD_POP_REAL();
  BARD_DISCARD_REF();
  BARD_PUSH_REAL(atan2(y,x));
}

/*
static void NativeData__clean_up()
{
  BardNativeData* data = (BardNativeData*) BARD_POP_REF();
  data->release();
}


static void ParseReader__prep_data()
{
  ParseReader__prep_data( BARD_POP_REF() );
}

static void ParseReader__has_another()
{
  BardLogical result = ParseReader__has_another( (BardParseReader*) BARD_POP_REF() );
  BARD_PUSH_INTEGER( result );
}

static void ParseReader__peek()
{
  BardChar result = ParseReader__peek( (BardParseReader*) BARD_POP_REF() );
  BARD_PUSH_INTEGER( result );
}

static void ParseReader__peek__Int32()
{
  BardInt32 num_ahead = (BardInt32) BARD_POP_INTEGER();
  BardChar result = ParseReader__peek__Int32( (BardParseReader*) BARD_POP_REF(), num_ahead );
  BARD_PUSH_INTEGER( result );
}

static void ParseReader__read()
{
  BardChar result = ParseReader__read( (BardParseReader*) BARD_POP_REF() );
  BARD_PUSH_INTEGER( result );
}

static void ParseReader__consume__Char()
{
  BardChar    ch = (BardChar) BARD_POP_INTEGER();
  BardLogical result = ParseReader__consume__Char( (BardParseReader*) BARD_POP_REF(), ch );
  BARD_PUSH_INTEGER( result );
}

static void ParseReader__consume__String()
{
  BardString* st = (BardString*) BARD_POP_REF();
  BardLogical result = ParseReader__consume__String( (BardParseReader*) BARD_POP_REF(), st );
  BARD_PUSH_INTEGER( result );
}


*/

static bool find_ip_info( BardInt64 ip, BardMethodInfo** m_ptr, char** filename_ptr, int* line_ptr )
{
  *m_ptr = NULL;
  *line_ptr = 0;
  *filename_ptr = NULL;

  int offset = ((BardOpcode*)(void*)ip) - bvm.code.data;
  for (int i=0; i<bvm.methods.count; ++i)
  {
    BardMethodInfo* m = bvm.methods[i];
    if (offset >= m->bytecode_offset && offset < m->bytecode_limit)
    {
      *m_ptr = m;
      break;
    }
  }

  if (*m_ptr == 0) return false;

  int best_filename_index = -1;
  int best_line = 0;
  for (int i=(*m_ptr)->source_pos_offset; i<bvm.line_table.count; ++i)
  {
    BardSourcePos pos = bvm.line_table[i];
    if (pos.code_offset >= offset) break;
    if (pos.source_pos > 0) best_line = pos.source_pos;
    else best_filename_index = -pos.source_pos;
  }

  if (best_filename_index == -1) return false;

  *filename_ptr = bvm.filenames[best_filename_index];
  *line_ptr = best_line;

  return true;
}

void StackTrace__describe__Int64()
{
  BardInt64 ip = BARD_POP_INTEGER();
  BARD_DISCARD_REF();

  BardMethodInfo* m;
  char* filename;
  int line;
  if (find_ip_info(ip,&m,&filename,&line))
  {
    StringBuilder buffer;
    buffer.print(line);
    buffer.print("@");
    buffer.print(filename);
    buffer.print("|");
    buffer.print(m->type_context->name);
    buffer.print("::");
    buffer.print(m->signature);
    BARD_PUSH_REF( (BardObject*) BardString::create(buffer.data,buffer.count) );
  }
  else
  {
    BARD_PUSH_REF( (BardObject*) BardString::create("[internal]") );
  }
}

static void get_call_history( ArrayList<BardInt64> &history )
{
  if (bvm.exception_ip)
  {
    history.add( (BardInt64)(void*)bvm.exception_ip );
    bvm.exception_ip = NULL;
  }

  BardCallFrame* cur_frame = bvm.regs.call_frame;
  while (cur_frame != bvm.call_stack_limit)
  {
    history.add( (BardInt64)(void*)((cur_frame++)->return_address) );
  }
  history.remove_last();
}

void StackTrace__native_history()
{
  BARD_DISCARD_REF();
  ArrayList<BardInt64> history;
  get_call_history(history);

  BardArray* array = bvm.type_array_of_int64->create( history.count );
  memcpy( array->data, history.data, history.count * 8 );
  BARD_PUSH_REF( array );
}

/*
void StdInReader__native_read_char()
{
  BARD_DISCARD_REF();
  BARD_PUSH_INTEGER( getc(stdin) );
}

static void StdOutWriter__print__Char()
{
  StdOutWriter__print__Char( NULL, (BardChar) BARD_POP_INTEGER() );
  BARD_DISCARD_REF();
}

static void StdOutWriter__print__String()
{
  StdOutWriter__print__String( NULL, (BardString*) BARD_POP_REF() );
  BARD_DISCARD_REF();
}

static void StringBuilder__native_copy()
{
  BardInt32 dest_offset = (BardInt32) BARD_POP_INTEGER();
  BardArray*  dest_array = (BardArray*) BARD_POP_REF();
  BardObject* src_string = BARD_POP_REF();
  BARD_DISCARD_REF();

  StringBuilder__native_copy__String_Array_of_Char_Int32( NULL, 
      src_string, dest_array, dest_offset );
}

static void StringManager__create_from__Array_of_Char_Int32()
{
  BardInt32   count = (BardInt32) BARD_POP_INTEGER();
  BardArray*  array = (BardArray*) BARD_POP_REF();
  BARD_DISCARD_REF();
  BARD_PUSH_REF( (BardObject*) 
      StringManager__create_from__Array_of_Char_Int32(NULL,array,count) );
}

static void StringManager__create_from__Char()
{
  BardChar chars[1];
  chars[0] = (BardChar) BARD_POP_INTEGER();
  BARD_DISCARD_REF();
  BARD_PUSH_REF( (BardObject*) BardString::create( chars, 1 ) );
}

static void String__count()
{
  BardString* st = (BardString*) BARD_POP_REF();
  BARD_PUSH_INTEGER( st->count );
}

static void String__hash_code()
{
  BardString* st = (BardString*) BARD_POP_REF();
  BARD_PUSH_INTEGER( st->hash_code );
}

static void String__get()
{
  BardInt32 index = (BardInt32) BARD_POP_INTEGER();
  BardString* st = (BardString*) BARD_POP_REF();
  BARD_PUSH_INTEGER( st->characters[index] );
}

static void String__opCMP()
{
  BardString* b = (BardString*) BARD_POP_REF();
  BardString* a = (BardString*) BARD_POP_REF();
  BardInt32 result = String__opCMP__String( a, b );
  BARD_PUSH_INTEGER( result );
}

static void String__opEQ()
{
  BardString* b = (BardString*) BARD_POP_REF();
  BardString* a = (BardString*) BARD_POP_REF();
  BardLogical result = String__opEQ__String( a, b );
  BARD_PUSH_INTEGER( result );
}

static void String__opADD__String()
{
  BardString* b = (BardString*) BARD_POP_REF();
  BardString* a = (BardString*) BARD_POP_REF();
  BardString* result = String__opADD__String( a, b );
  BARD_PUSH_REF( (BardObject*) result );
}

static void String__opADD__Char()
{
  BardChar ch = (BardChar) BARD_POP_INTEGER();
  BardString* st = (BardString*) BARD_POP_REF();
  BardString* result = String__opADD__Char( st, ch );
  BARD_PUSH_REF( (BardObject*) result );
}

static void String__substring()
{
  BardInt32 i2 = (BardInt32) BARD_POP_INTEGER();
  BardInt32 i1 = (BardInt32) BARD_POP_INTEGER();
  BardString* st = (BardString*) BARD_POP_REF();
  BardString* result = String__substring__Int32_Int32( st, i1, i2 );
  BARD_PUSH_REF( (BardObject*) result );
}

static void String__to_Array()
{
  BardObject* result = String__to_Array( (BardString*) BARD_POP_REF() );
  BARD_PUSH_REF( (BardObject*) result );
}

static void System__exit_program__Int32_String()
{
  BardInt32   result_code = (BardInt32) BARD_POP_INTEGER();
  BardString* mesg = (BardString*) BARD_POP_REF();
  if (mesg)
  {
    bard_error_message = mesg->to_new_ascii();
  }
  throw result_code;
}

static void System__get__String()
{
  BardString* env_var_name = (BardString*) BARD_POP_REF();
  BARD_DISCARD_REF();
  BardString* result = System__get__String( NULL, env_var_name );
  BARD_PUSH_REF( result );
}

static void System__os()
{
  BardString* result = System__os( BARD_POP_REF() );
  BARD_PUSH_REF( result );
}

static void System__raw_exe_filepath()
{
  BARD_DISCARD_REF();
  BardString* result = System__raw_exe_filepath( NULL );
  BARD_PUSH_REF( result );
}
*/

void BardVM::register_stdlib()
{
  register_native_method( "GenericArray", "clear(Int32,Int32)",   GenericArray__clear__Int32_Int32 );
  register_native_method( "GenericArray", "count()",   GenericArray__count );
  register_native_method( "GenericArray", "copy_from(GenericArray,Int32,Int32,Int32)",
      GenericArray__copy_from__GenericArray_Int32_Int32_Int32 );

  register_native_method( "Date", "init(Int64)", Date__init__Int64 );
  register_native_method( "Date", "timestamp()", Date__timestamp );

  register_native_method( "File", "exists()",            File__exists );
  register_native_method( "File", "is_directory()",      File__is_directory );
  register_native_method( "File", "absolute_filepath()", File__absolute_filepath );
  register_native_method( "File", "directory_listing(ArrayList<<String>>)", 
      File__directory_listing__ArrayList_of_String );
  register_native_method( "File", "rename(String)",      File__rename__String );
  register_native_method( "File", "delete()",            File__delete );
  register_native_method( "File", "timestamp()",         File__timestamp );
  register_native_method( "File", "touch()",             File__touch );
  register_native_method( "File", "native_mkdir()",      File__native_mkdir );
  register_native_method( "File", "change_dir()",        File__change_dir );

  register_native_method( "FileReader", "init(String)",  FileReader__init__String );
  register_native_method( "FileReader", "close()",       FileReader__close );
  register_native_method( "FileReader", "has_another()", FileReader__has_another );
  register_native_method( "FileReader", "peek()",        FileReader__peek );
  register_native_method( "FileReader", "read()",        FileReader__read );
  register_native_method( "FileReader", "read(Array<<Byte>>,Int32,Int32)", 
      FileReader__read__Array_of_Byte_Int32_Int32 );
  register_native_method( "FileReader", "read(Array<<Char>>,Int32,Int32)", 
      FileReader__read__Array_of_Char_Int32_Int32 );
  register_native_method( "FileReader", "remaining()",   FileReader__remaining );
  register_native_method( "FileReader", "skip(Int32)",   FileReader__skip__Int32 );
  register_native_method( "FileReader", "position()",    FileReader__position );

  register_native_method( "FileWriter", "init(String,Logical)", FileWriter__init__String_Logical );
  register_native_method( "FileWriter", "close()",       FileWriter__close );
  register_native_method( "FileWriter", "write(Char)",   FileWriter__write__Char );
  register_native_method( "FileWriter", "write(Array<<Char>>,Int32,Int32)",   
      FileWriter__write__Array_of_Char_Int32_Int32 );
  register_native_method( "FileWriter", "write(Array<<Byte>>,Int32,Int32)",   
      FileWriter__write__Array_of_Byte_Int32_Int32 );
  register_native_method( "FileWriter", "write(String)", FileWriter__write__String );
  register_native_method( "FileWriter", "position()",    FileWriter__position );

  register_native_method( "WeakReferenceManager", "create_from(Object)", 
      WeakReferenceManager__create_from__Object );
  register_native_method( "WeakReference", "object(Object)", 
      WeakReference__object__Object );
  register_native_method( "WeakReference", "object()", 
      WeakReference__object );

  register_native_method( "Global", "time_ms()",    Global__time_ms );
  register_native_method( "Global", "sleep(Int32)", Global__sleep__Int32 );

  register_native_method( "Math",   "abs(Int64)",  Math__abs__Int64 );
  register_native_method( "Math",   "abs(Real64)", Math__abs__Real64 );
  register_native_method( "Math",   "floor(Real64)", Math__floor__Real64 );
  register_native_method( "Math",   "sqrt(Real64)", Math__sqrt__Real64 );
  register_native_method( "Math",   "sin(Radians)", Math__sin__Radians );
  register_native_method( "Math",   "cos(Radians)", Math__cos__Radians );
  register_native_method( "Math",   "tan(Radians)", Math__tan__Radians );
  register_native_method( "Math",   "asin(Real64)", Math__asin__Real64 );
  register_native_method( "Math",   "acos(Real64)", Math__acos__Real64 );
  register_native_method( "Math",   "atan(Real64)", Math__atan__Real64 );
  register_native_method( "Math",   "atan2(Real64,Real64)", Math__atan2__Real64_Real64 );

  register_native_method( "NativeData", "clean_up()", NativeData__clean_up );

  register_native_method( "Network", "is_connected()", Network__is_connected );
  register_native_method( "Network", "is_mobile()",    Network__is_mobile );
  register_native_method( "Network", "mac_address()",  Network__mac_address );

  register_native_method( "Object", "hash_code()", Object__hash_code );
  /*
  register_native_method( "Object", "runtime_type()", Object__runtime_type );
  */

  register_native_method( "ParseReader", "prep_data()",   ParseReader__prep_data );
  register_native_method( "ParseReader", "has_another()", ParseReader__has_another );
  register_native_method( "ParseReader", "peek()",        ParseReader__peek );
  register_native_method( "ParseReader", "peek(Int32)",   ParseReader__peek__Int32 );
  register_native_method( "ParseReader", "read()",        ParseReader__read );
  register_native_method( "ParseReader", "consume(Char)",   ParseReader__consume__Char );
  register_native_method( "ParseReader", "consume(String)", ParseReader__consume__String );

  register_native_method( "Process", "init(String)", Process__init__String );
  register_native_method( "Process", "update()",     Process__update );
  register_native_method( "Process", "release()",    Process__release );

  /*
  register_native_method( "Runtime", "find_type_index(String)", Runtime__find_type_index__String );

  register_native_method( "RuntimeMethods", "count()", RuntimeMethods__count );

  register_native_method( "RuntimeMethod", "name()", RuntimeMethod__name );
  register_native_method( "RuntimeMethod", "return_type()", RuntimeMethod__return_type );
  register_native_method( "RuntimeMethod", "parameter_types()", RuntimeMethod__parameter_types );
  register_native_method( "RuntimeMethod", "signature()", RuntimeMethod__signature );
  register_native_method( "RuntimeMethod", "arg(Object)", RuntimeMethod__arg__Object );
  register_native_method( "RuntimeMethod", "arg(Int64)",  RuntimeMethod__arg__Int64 );
  register_native_method( "RuntimeMethod", "arg(Int32)",  RuntimeMethod__arg__Int32 );
  register_native_method( "RuntimeMethod", "call()", RuntimeMethod__call );
  register_native_method( "RuntimeMethod", "call_Object()", RuntimeMethod__call_Object );
  register_native_method( "RuntimeMethod", "call_Int64()",  RuntimeMethod__call_Int64 );
  register_native_method( "RuntimeMethod", "call_Int32()",  RuntimeMethod__call_Int32 );

  register_native_method( "RuntimeProperties", "count()", RuntimeProperties__count );

  register_native_method( "RuntimeProperty", "type()", RuntimeProperty__type );
  register_native_method( "RuntimeProperty", "name()", RuntimeProperty__name );
  register_native_method( "RuntimeProperty", "as_Object()", RuntimeProperty__as_Object );
  register_native_method( "RuntimeProperty", "as_Object(Object)", RuntimeProperty__as_Object__Object );
  register_native_method( "RuntimeProperty", "as_Int64()", RuntimeProperty__as_Int64 );
  register_native_method( "RuntimeProperty", "as_Int64(Int64)", RuntimeProperty__as_Int64__Int64 );
  register_native_method( "RuntimeProperty", "as_Int32()", RuntimeProperty__as_Int32 );
  register_native_method( "RuntimeProperty", "as_Int32(Int32)", RuntimeProperty__as_Int32__Int32 );
  register_native_method( "RuntimeProperty", "as_Char()", RuntimeProperty__as_Char );
  register_native_method( "RuntimeProperty", "as_Char(Char)", RuntimeProperty__as_Char__Char );
  register_native_method( "RuntimeProperty", "as_Byte()", RuntimeProperty__as_Byte );
  register_native_method( "RuntimeProperty", "as_Byte(Byte)", RuntimeProperty__as_Byte__Byte );

  register_native_method( "RuntimeType", "name()", RuntimeType__name );
  register_native_method( "RuntimeType", "instance_of(RuntimeType)", RuntimeType__instance_of__RuntimeType );
  register_native_method( "RuntimeType", "create_instance()", RuntimeType__create_instance );
  */

  register_native_method( "SHA1", "hmac(String,String)",  SHA1__hmac__String_String );

  register_native_method( "Socket", "native_init(String,Int32)",  Socket__native_init__String_Int32 );
  register_native_method( "Socket", "connection_pending()",Socket__connection_pending );
  register_native_method( "Socket", "is_connected()",      Socket__is_connected );
  register_native_method( "Socket", "native_remote_ip()",  Socket__native_remote_ip );
  register_native_method( "Socket", "close()",             Socket__close );
  register_native_method( "SocketReader", "available()",   SocketReader__available );
  register_native_method( "SocketReader", "peek()",        SocketReader__peek );
  register_native_method( "SocketReader", "read()",        SocketReader__read );
  register_native_method( "SocketWriter", "write(Char)",   SocketWriter__write__Char );
  register_native_method( "ServerSocket", "native_init(Int32)", ServerSocket__native_init__Int32 );
  register_native_method( "ServerSocket", "is_connected()",ServerSocket__is_connected );
  register_native_method( "ServerSocket", "get_pending_info()", ServerSocket__get_pending_info );
  register_native_method( "ServerSocket", "close()",       ServerSocket__close );

  register_native_method( "StackTrace", "describe(Int64)", StackTrace__describe__Int64 );
  register_native_method( "StackTrace", "native_history()", StackTrace__native_history );

  register_native_method( "StdInReader", "native_read_char()", StdInReader__native_read_char );

  register_native_method( "StdOutWriter", "print(Char)",   StdOutWriter__print__Char );
  register_native_method( "StdOutWriter", "print(String)", StdOutWriter__print__String );
  register_native_method( "StdOutWriter", "write(Char)",   StdOutWriter__print__Char );
  register_native_method( "StdOutWriter", "write(String)", StdOutWriter__print__String );
  register_native_method( "StdOutWriter", "flush()",       StdOutWriter__flush );

  register_native_method( "StringBuilder", "native_copy(String,Array<<Char>>,Int32)", 
      StringBuilder__native_copy__String_Array_of_Char_Int32 );

  register_native_method( "StringManager", "create_from(Array<<Char>>,Int32)", 
      StringManager__create_from__Array_of_Char_Int32 );
  register_native_method( "StringManager", "create_from(Char)", StringManager__create_from__Char );

  register_native_method( "String", "count()", String__count );
  register_native_method( "String", "hash_code()", String__hash_code );
  register_native_method( "String", "get(Int32)", String__get__Int32 );
  register_native_method( "String", "op+(String)", String__opADD__String );
  register_native_method( "String", "op+(Char)", String__opADD__Char );
  register_native_method( "String", "op<>(String)", String__opCMP__String );
  register_native_method( "String", "op==(String)", String__opEQ__String );
  register_native_method( "String", "substring(Int32,Int32)", String__substring__Int32_Int32 );
  register_native_method( "String", "to_Array()", String__to_Array );

  register_native_method( "System", "catch_control_c(Logical)", System__catch_control_c__Logical );
  register_native_method( "System", "control_c_pressed()", System__control_c_pressed );

  register_native_method( "System", "exit_program(Int32,String)", 
      System__exit_program__Int32_String );

  //register_native_method( "System", "execution_ip(Int32)", System__execution_ip );
  /*
  register_native_method( "System", "describe_execution_ip(Int64)", System__describe_execution_ip__Int64 );
  register_native_method( "System", "call_history()", System__call_history );
  */

  register_native_method( "System", "force_garbage_collection()", System__force_garbage_collection );
  register_native_method( "System", "get(String)", System__get__String );

  register_native_method( "System", "os()", System__os );

  register_native_method( "System", "os_version()", System__os_version );
  register_native_method( "System", "device_type()", System__device_type );
  register_native_method( "System", "language()", System__language );
  //register_native_method( "System", "ip()", System__ip );
  register_native_method( "System", "raw_exe_filepath()", System__raw_exe_filepath );
}

void BardVM::apply_hooks()
{
  for (int i=0; i<bvm.native_method_hooks.count; ++i)
  {
    const char* class_name = bvm.native_method_hooks[i].class_name;
    const char* sig = bvm.native_method_hooks[i].signature;
    BardNativeFn fn = bvm.native_method_hooks[i].fn;
    BardTypeInfo* type;
    BardMethodInfo *m;
    type = bvm.find_type( class_name );
    if ( !type ) continue;

    m = type->find_method( sig );
    if ( m ) 
    {
      m->native_handler = fn;
    }
    else
    {
      // Make sure we're not failing due to accidental inclusion of return type
      const char* cur = sig;
      while (*cur)
      {
        if (*(cur++)=='.') 
        {
          bard_throw_fatal_error( "Illegal specification of return type in native method hook." );
        }
      }
      continue;
    }
  }
  native_method_hooks.clear();
}


void BardVM::set_command_line_args( char* args[], int count )
{
  BardObject* list = ((BardGenericObject*)(bvm.must_find_type("Global")->singleton()))->get_ref("command_line_args");
  BardMethodInfo* m_add = list->type->must_find_method("add(String)");
  for (int i=0; i<count; ++i)
  {
    BARD_PUSH_REF( list );
    BARD_PUSH_REF( (BardObject*) BardString::create(args[i]) );
    bvm.call( m_add );
    BARD_DISCARD_REF();
  }
}

void BardVM::prep_types_and_methods()
{
  int num_singletons = 0;
  for (int t=0; t<bvm.types.count; ++t)
  {
    BardTypeInfo* type = bvm.types[t];
	  
    if (type->qualifiers & BARD_QUALIFIER_SINGLETON)
    {
      type->singleton_index = num_singletons++;
    }

    if (type->qualifiers & BARD_QUALIFIER_PRIMITIVE)
    {
      type->stack_slots = 1;
      if (type == bvm.type_int64)
      {
        type->object_size    = 8;
      }
      else if (type == bvm.type_int32)
      {
        type->object_size    = 4;
      }
      else if (type == bvm.type_char)
      {
        type->object_size    = 4;
      }
      else if (type == bvm.type_byte)
      {
        type->object_size    = 4;
      }
      else if (type == bvm.type_real64)
      {
        type->object_size    = 8;
      }
      else if (type == bvm.type_real32)
      {
        type->object_size    = 4;
      }
      else if (type == bvm.type_logical)
      {
        type->object_size    = 4;
      }
      else 
      {
        bard_throw_fatal_error( "Unknown primitive type." );
      }
    }
    else
    {
      if (type->is_reference())
      {
        type->object_size = sizeof(BardObject);
        type->stack_slots = sizeof(void*);

        if (type->instance_of(bvm.type_requires_cleanup) && type != bvm.type_null)
        {
          type->qualifiers |= BARD_QUALIFIER_REQUIRES_CLEANUP;
        }
      }
      else type->object_size = 0;

      for (int p=0; p<type->properties.count; ++p)
      {
        BardPropertyInfo* property = type->properties[p];
        if (property->type->is_reference())
        {
          while (type->object_size & (sizeof(void*)-1)) ++type->object_size;
          property->offset = type->object_size;
          type->reference_property_offsets.add( type->object_size );
          type->object_size += sizeof(void*);
        }
        else 
        {
          if (type->is_compound())
          {
            int slots = property->type->stack_slots;
            property->offset = type->object_size >> 3;
            type->object_size += slots << 3;
          }
          else
          {
            int bytes = property->type->object_size;
            if (bytes == 2)
            {
              if (type->object_size & 1) ++type->object_size;
            }
            else if (bytes == 4)
            {
              while (type->object_size & 3) ++type->object_size;
            }
            else if (bytes >= 8)
            {
              if (offsetof(BardAlignmentInfo,real) == 8)
              {
                while (type->object_size & 7) ++type->object_size;
              }
              else
              {
                while (type->object_size & 3) ++type->object_size;
              }
            }
            property->offset = type->object_size;
            type->object_size += bytes;
          }
        }
      }

      // round final size up to multiple of 8 bytes
      while (type->object_size & 7) ++type->object_size;

      if (type->is_compound())
      {
        type->stack_slots = type->object_size/8;
      }
    }
  }

  // Create singleton data.
  bvm.singletons.ensure_capacity( num_singletons );
  bvm.singletons.count = num_singletons;

  // Prep methods
  // Fill in parameter & local var offsets.
  // Stacks grow downward.  Stack frames are set up so that the first
  // parameters are below frame[0].
  //
  // References:
  //   "this"          - ref_frame_ptr[ -1 ]
  //   first ref param - ref_frame_ptr[ -2 ]
  //   last ref param  - ref_frame_ptr[ -(num_ref_params+1) ]
  //   first local ref - ref_frame_ptr[ -(num_ref_params+2) ]
  //   last local ref  - ref_frame_ptr[ -(num_ref_params+num_local_refs+1) ]
  //
  // Value data is similar.
  for (int j=0; j<bvm.parameter_table.count; ++j)
  {
    BardParameterList& params = *bvm.parameter_table[j];

    int   ref_offset = 0;
    int   data_offset = 0;

    --ref_offset; // for "this"
    ++params.num_ref_params;

    for (int i=0; i<params.count(); ++i)
    {
      BardTypeInfo* type = params[i]->type;
      if (type->is_reference())
      {
        params[i]->offset = --ref_offset;
        ++params.num_ref_params;
      }
      else
      {
        data_offset -= type->stack_slots;
        params[i]->offset = data_offset;
        params.num_data_params += type->stack_slots;
      }
    }

  }

  for (int j=0; j<bvm.methods.count; ++j)
  {
    BardMethodInfo* m = bvm.methods[j];
    int ref_offset = -(m->parameters->num_ref_params);
    int data_offset = -(m->parameters->num_data_params);
    for (int i=0; i<m->local_vars.count; ++i)
    {
      BardTypeInfo* type = m->local_vars[i]->type;
      if (type->is_reference())
      {
        m->local_vars[i]->offset = --ref_offset;
        ++m->num_local_refs;
      }
      else
      {
        data_offset -= type->stack_slots;
        m->local_vars[i]->offset = data_offset;
        m->local_data_size += type->stack_slots;
      }
    }

    // Keep stack 8-byte aligned.
    while (m->local_data_size & 7) ++m->local_data_size;

    m->create_signature();
  }

  // Find some common methods for each type.
  for (int t=0; t<bvm.types.count; ++t)
  {
    BardTypeInfo* type = bvm.types[t];

    type->method_init_object = type->find_method( "init_object()" );
    if (type->requires_cleanup())
    {
      type->method_clean_up = type->find_method( "clean_up()" );
      // Find instead of must_find - if a RequiresCleanup type doesn't have a clean_up()
      // method, that means no objects of that type are created.
    }
  }

  // Specific other methods & properties
  //bvm.method_String_set_hash_code  = bvm.type_string->must_find_method( "set_hash_code()" );
}


void BardVM::configure()
{
  register_stdlib();

  apply_hooks();

  int ref_stack_count  = BARD_STACK_SIZE;   // 8k
  int data_stack_count = BARD_STACK_SIZE;
  int call_stack_count = BARD_STACK_SIZE/2; // 4k

  if (ref_stack)
  {
    printf( "ERROR: ref_stack already initialized!\n" );
  }

  ref_stack = new BardObject*[ref_stack_count];
  data_stack = new BardInt64[data_stack_count];
  call_stack = new BardCallFrame[call_stack_count];

  min_ref_stack  = ref_stack + 16;
  min_data_stack = data_stack + 16;
  min_call_stack = call_stack + 1;

  ref_stack_limit = ref_stack + ref_stack_count;
  data_stack_limit = data_stack + data_stack_count;
  call_stack_limit = call_stack + call_stack_count;

  regs.stack.data  = data_stack_limit;
  regs.stack.refs  = ref_stack_limit;

  regs.frame.data  = data_stack_limit;
  regs.frame.refs  = ref_stack_limit;

  regs.call_frame = call_stack_limit;

  // Set the hash code of each string literal.
  //for (int i=0; i<bvm.strings.count; ++i)
  //{
    //BARD_PUSH_REF( bvm.strings[i] );
    //bvm.call( bvm.method_String_set_hash_code );
  //}

  create_singletons();

  /*
  bvm_pass_command_line_args();
  */
}

void BardVM::create_singletons()
{
  // Allocate memory for each singleton without calling
  // init_object() or init(). 
  for (int i=0; i<types.count; ++i)
  {
    BardTypeInfo* type = types[i];
    if (type->is_singleton())
    {
      int index = type->singleton_index;
      singletons[index] = (mm.create_object( type, type->object_size ));
      ++singletons[index]->reference_count;
    }
  }

  // Create the main object without calling init_object() or init()
  BardObject* main_obj;

  if (main_class->is_singleton())
  {
    main_obj = main_class->singleton();
  }
  else if (main_class->is_managed())
  {
    char buffer[512];
    sprintf( buffer, "%sManager", main_class->name );
    BARD_FIND_TYPE( mgr_type, buffer );
    BardObject* mgr_obj = mgr_type->singleton();

    main_obj = main_class->create();
    BARD_SET_REF( mgr_obj, "singleton_instance", main_obj );
  }
  else
  {
    main_obj = main_class->create();
  }

  bard_main_object = main_obj;

  // Call Global.set_up_stdio() first so that any error messages
  // generated during general singleton instantatiation can 
  // be displayed.
  BardTypeInfo* type_global = must_find_type( "Global" );
  BARD_PUSH_REF( type_global->singleton() );
  BARD_CALL( type_global, "set_up_stdio()" );

  // Call init_object() on each singleton.  Don't call
  // init_object() on the main object if it happens to
  // be a singleton.
  for (int i=0; i<bvm.types.count; ++i)
  {
    BardTypeInfo* type = bvm.types[i];
    if (type->is_singleton() && type != bvm.main_class)
    {
      if (type->method_init_object)
      {
        // Having only null references to unused singletons may 
        // result in a singleton that doesn't have an init_object() 
        // method.
        BARD_PUSH_REF( type->singleton() );
        call( type->method_init_object );
      }
    }
  }

  // Call init_singleton() on each singleton if it exists.
  for (int i=0; i<bvm.types.count; ++i)
  {
    BardTypeInfo* type = bvm.types[i];
    if (type->is_singleton())
    {
      BardMethodInfo* m_init = type->find_method("init_singleton()");
      if (m_init)
      {
        BARD_PUSH_REF( type->singleton()  );
        call( m_init );
      }
    }
  }
}

void BardVM::call( BardMethodInfo* method )
{
  static BardOpcode halt_instruction = BARDOP_HALT;

  if ( method )
  {
    BardOpcode* prior_ip = bvm.regs.ip;
    bvm.regs.ip = &halt_instruction;

    invoke( method );
    execute();
    bvm.regs.ip = prior_ip;
  }
  else
  {
    bard_throw_fatal_error( "Null method called." );
  }
}

void BardVM::invoke( BardMethodInfo* method )
{
  // Save prior call frame.
  (--bvm.regs.call_frame)->return_address = bvm.regs.ip;
  bvm.regs.call_frame->called_method = method;
  bvm.regs.call_frame->prior_frame = bvm.regs.frame;

  // Set up current call frame pointers to be just before parameter data.
  bvm.regs.frame.refs  = bvm.regs.stack.refs  + method->parameters->num_ref_params;
  bvm.regs.frame.data = bvm.regs.stack.data + method->parameters->num_data_params;

  // Adjust stack pointers to make room for local data & clear that region.
  {
    int count = method->num_local_refs;
    if (count)
    {
      bvm.regs.stack.refs -= count;
      memset( bvm.regs.stack.refs, 0, count*sizeof(BardObject*) );
      // need to clear refs so they don't screw up a GC
    }

    bvm.regs.stack.data -= method->local_data_size;
    // Don't need to clear data since every local var is zeroed by default
    // if no other initial assignment is given.
  }
  bvm.regs.ip = method->bytecode;
}


void BardVM::execute()
{
  BardMethodInfo *m = NULL;

  for (;;)
  {
    switch (*(bvm.regs.ip++))
    {
      case BARDOP_HALT:
        return;

      case BARDOP_NOP:
        continue;

      case BARDOP_BREAKPOINT:
        {
          BardInt32 index = BARDCODE_OPERAND_I32();
          printf( "Breakpoint %d\n", index );		
          continue;
        }       

      case BARDOP_NATIVE_CALL:
        {
          m = bvm.methods[BARDCODE_OPERAND_I32()];

          // debug stack guard saves a lot of headache
          BardOpcode* next_ip = bvm.regs.ip;
          BardObject** ref_stack = bvm.regs.stack.refs + m->parameters->num_ref_params;
          BardInt64* data_stack = bvm.regs.stack.data + m->parameters->num_data_params;
          if (m->return_type)
          {
            if (m->return_type->is_reference()) --ref_stack;
            else data_stack -= m->return_type->stack_slots;
          }

          m->native_handler();

          if ((bvm.regs.stack.refs != ref_stack || bvm.regs.stack.data != data_stack)
              && bvm.regs.ip == next_ip)
          {
            if (bvm.regs.stack.refs != ref_stack) printf( "Corrupted reference stack" );
            else                                  printf( "Corrupted data stack" );
            printf( " in %s::%s\n",m->type_context->name,m->signature );
          }
        }
        continue;

      case BARDOP_MISSING_RETURN_ERROR:
        BVM_THROW_TYPE( bvm.type_missing_return_error, continue );

      case BARDOP_RETURN_NIL:
        bvm.regs.stack = bvm.regs.frame;
        bvm.regs.frame = bvm.regs.call_frame->prior_frame;
        bvm.regs.ip = (bvm.regs.call_frame++)->return_address;
        continue;

      case BARDOP_RETURN_REF:
        {
          BardObject* result  = BARD_POP_REF();
          bvm.regs.stack      = bvm.regs.frame;
          bvm.regs.frame      = bvm.regs.call_frame->prior_frame;
          bvm.regs.ip = (bvm.regs.call_frame++)->return_address;
          BARD_PUSH_REF( result );
        }
        continue;

      case BARDOP_RETURN_8:
        {
          BardInt64 result = BARD_POP_INTEGER();
          bvm.regs.stack   = bvm.regs.frame;
          bvm.regs.frame   = bvm.regs.call_frame->prior_frame;
          bvm.regs.ip = (bvm.regs.call_frame++)->return_address;
          BARD_PUSH_INTEGER( result );
          continue;
        }

      case BARDOP_RETURN_X:
        {
          BardInt64* from_spot = bvm.regs.stack.data;
          BardInt32 size  = BARDCODE_OPERAND_I32();
          bvm.regs.stack  = bvm.regs.frame;
          bvm.regs.frame  = bvm.regs.call_frame->prior_frame;
          bvm.regs.ip = (bvm.regs.call_frame++)->return_address;
          bvm.regs.stack.data -= size;
          memmove( bvm.regs.stack.data, from_spot, size<<3 );
        }
        continue;

      case BARDOP_DUPLICATE_REF:
        {
          BardObject* ref = *bvm.regs.stack.refs;
          BARD_PUSH_REF( ref );
        }
        continue;

      case BARDOP_DUPLICATE_8:
        {
          BardInt64 n = *(bvm.regs.stack.data);
          BARD_PUSH_INTEGER( n );
          continue;
        }

      case BARDOP_POP_REF:
        ++bvm.regs.stack.refs;
        continue;

      case BARDOP_POP_8:
        ++bvm.regs.stack.data;
        continue;

      case BARDOP_POP_X:
        {
          BardInt32 size = BARDCODE_OPERAND_I32();
          bvm.regs.stack.data += size;
          continue;
        }

      case BARDOP_JUMP:
        // Can't use OPERAND_ADDR() here since it would increment bvm.regs.ip
        // after we assigned it.
        bvm.regs.ip = (BardOpcode*) bvm.address_table[*bvm.regs.ip];
        continue;

      case BARDOP_JUMP_IF_TRUE:
        {
          BardOpcode *dest = (BardOpcode*) BARDCODE_OPERAND_ADDR();
          if (BARD_POP_INTEGER()>0) bvm.regs.ip = dest;
        }
        continue;

      case BARDOP_JUMP_IF_FALSE:
        {
          BardOpcode *dest = (BardOpcode*) BARDCODE_OPERAND_ADDR();
          if (BARD_POP_INTEGER()<=0) bvm.regs.ip = dest;
        }
        continue;

      case BARDOP_JUMP_IF_REF:
        {
          BardOpcode *dest = (BardOpcode*) BARDCODE_OPERAND_ADDR();
          if (BARD_POP_REF()) bvm.regs.ip = dest;
        }
        continue;

      case BARDOP_JUMP_IF_NULL_REF:
        {
          BardOpcode *dest = (BardOpcode*) BARDCODE_OPERAND_ADDR();
          if ( !BARD_POP_REF() ) bvm.regs.ip = dest;
        }
        continue;

      case BARDOP_THROW:
        throw_exception_on_stack();
        continue;

      case BARDOP_LITERAL_STRING:
        BARD_PUSH_REF( bvm.strings[BARDCODE_OPERAND_I32()] );
        continue;

      case BARDOP_LITERAL_NULL:
        BARD_PUSH_REF( NULL );
        continue;

      case BARDOP_LITERAL_8:
        BARD_PUSH_INTEGER( BARDCODE_OPERAND_I64() );
        continue;

      case BARDOP_LITERAL_4:
        BARD_PUSH_INTEGER( BARDCODE_OPERAND_I32() );
        continue;

      case BARDOP_LITERAL_REAL32:
        {
          BardInt32 n = BARDCODE_OPERAND_I32();
          BARD_PUSH_REAL( *((BardReal32*)&n) );
        }
        continue;

      case BARDOP_LITERAL_INTEGER_1:
        BARD_PUSH_INTEGER( 1 );
        continue;

      case BARDOP_LITERAL_INTEGER_0:
        BARD_PUSH_INTEGER( 0 );
        continue;

      case BARDOP_LITERAL_INTEGER_NEG1:
        BARD_PUSH_INTEGER( -1 );
        continue;

      case BARDOP_LITERAL_REAL_1:
        BARD_PUSH_REAL( 1 );
        continue;

      case BARDOP_LITERAL_REAL_0:
        BARD_PUSH_REAL( 0 );
        continue;

      case BARDOP_LITERAL_REAL_NEG1:
        BARD_PUSH_REAL( -1 );
        continue;

      case BARDOP_READ_THIS_REF:
        BARD_PUSH_REF( bvm.regs.frame.refs[-1] );
        continue;

      case BARDOP_READ_SINGLETON_REF:
        BARD_PUSH_REF( bvm.singletons[BARDCODE_OPERAND_I32()] );
        continue;

      case BARDOP_READ_PROPERTY_REF:
        {
          BardObject* context = BARD_POP_REF( );
          BVM_NULL_CHECK( context, continue );
          BARD_PUSH_REF( BVM_DEREFERENCE(context,BARDCODE_OPERAND_I32(),BardObject*) );
        }
        continue;

      case BARDOP_READ_PROPERTY_1U:
        BARDCODE_READ_PROPERTY( BardByte );
        continue;

      case BARDOP_READ_PROPERTY_2U:
        BARDCODE_READ_PROPERTY( BardChar );
        continue;

      case BARDOP_READ_PROPERTY_4:
        BARDCODE_READ_PROPERTY( BardInt32 );
        continue;

      case BARDOP_READ_PROPERTY_REAL32:
        {
          BardObject* context = BARD_POP_REF();
          BVM_ASSERT( context, bvm.type_null_reference_error, continue );
          BARD_PUSH_REAL( BVM_DEREFERENCE(context,BARDCODE_OPERAND_I32(),BardReal32) );
        }
        continue;

      case BARDOP_READ_PROPERTY_8:
        BARDCODE_READ_PROPERTY( BardInt64 );
        continue;

      case BARDOP_READ_PROPERTY_X:
        {
          int offset, size;
          BardObject* context = BARD_POP_REF();
          BVM_NULL_CHECK( context, continue );
          offset = BARDCODE_OPERAND_I32();
          size = BARDCODE_OPERAND_I32();
          bvm.regs.stack.data -= size>>3;
          memcpy( bvm.regs.stack.data, ((char*)context)+offset, size );
        }
        continue;

      case BARDOP_READ_THIS_PROPERTY_REF:
        BARD_PUSH_REF( BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),BardObject*) );
        continue;

      case BARDOP_READ_THIS_PROPERTY_4:
        BARD_PUSH_INTEGER( BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),BardInt32) );
        continue;

      case BARDOP_READ_THIS_PROPERTY_REAL32:
        BARD_PUSH_REAL( BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),BardReal32) );
        continue;

      case BARDOP_READ_THIS_PROPERTY_8:
        BARD_PUSH_INTEGER( BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),BardInt64) );
        continue;

      case BARDOP_READ_THIS_PROPERTY_X:
        {
          BardInt32 offset = BARDCODE_OPERAND_I32();
          BardInt32 size = BARDCODE_OPERAND_I32();
          bvm.regs.stack.data -= size>>3;
          memcpy( bvm.regs.stack.data, ((char*)bvm.regs.frame.refs[-1])+offset, size );
        }
        continue;

      case BARDOP_READ_LOCAL_REF:
        BARD_PUSH_REF( bvm.regs.frame.refs[BARDCODE_OPERAND_I32()] );
        continue;

      case BARDOP_READ_LOCAL_8:
        BARD_PUSH_INTEGER( bvm.regs.frame.data[BARDCODE_OPERAND_I32()] );
        continue;

      case BARDOP_READ_LOCAL_X:
        {
          BardInt32 offset = (BardInt32) BARDCODE_OPERAND_I32();
          BardInt32 slots  = (BardInt32) BARDCODE_OPERAND_I32();
          bvm.regs.stack.data -= slots;
          memcpy( bvm.regs.stack.data, bvm.regs.frame.data+offset, slots<<3 );
          continue;
        }

      case BARDOP_READ_COMPOUND_8:
        {
          BardInt32 total_slots = BARDCODE_OPERAND_I32();
          BardInt32 offset     = BARDCODE_OPERAND_I32();
          BardInt64 i64 = bvm.regs.stack.data[offset];
          bvm.regs.stack.data += total_slots;
          BARD_PUSH_INTEGER( i64 );
        }
        continue;

      case BARDOP_READ_COMPOUND_X:
        {
          BardInt32 total_slots = BARDCODE_OPERAND_I32();
          BardInt32 offset      = BARDCODE_OPERAND_I32();
          BardInt32 slots       = BARDCODE_OPERAND_I32();
          BardInt64* source_ptr = bvm.regs.stack.data + offset;
          bvm.regs.stack.data += (total_slots - slots);
          memmove( bvm.regs.stack.data, source_ptr, slots<<3 );
        }
        continue;

      case BARDOP_WRITE_SINGLETON_REF:
        {
          BardObject*  new_value = BARD_POP_REF();
          BardObject** dest = bvm.singletons.data + BARDCODE_OPERAND_I32();
          if (*dest) --((*dest)->reference_count);
          if (new_value) ++(new_value->reference_count);
          *dest = new_value;
        }
        continue;

      case BARDOP_WRITE_PROPERTY_REF:
        {
          BardObject* new_value = BARD_POP_REF();
          BardObject* context = BARD_POP_REF();
          BVM_NULL_CHECK( context, continue );
          BardInt32   offset = BARDCODE_OPERAND_I32();
          BardObject** dest = (BardObject**)(((char*)context)+offset);
          if (*dest) --((*dest)->reference_count);
          if (new_value) ++(new_value->reference_count);
          *dest = new_value;
        }
        continue;

      case BARDOP_WRITE_PROPERTY_1:
        BARDCODE_WRITE_PROPERTY( BardInt8 );
        continue;

      case BARDOP_WRITE_PROPERTY_2:
        BARDCODE_WRITE_PROPERTY( BardInt16 );
        continue;

      case BARDOP_WRITE_PROPERTY_4:
        BARDCODE_WRITE_PROPERTY( BardInt32 );
        continue;

      case BARDOP_WRITE_PROPERTY_REAL32:
        {
          BardReal64 new_value = BARD_POP_REAL();
          BardObject* context = BARD_POP_REF();
          BVM_ASSERT( context, bvm.type_null_reference_error, continue );
          BVM_DEREFERENCE(context,BARDCODE_OPERAND_I32(),BardReal32) = (BardReal32) new_value;
        }
        continue;

      case BARDOP_WRITE_PROPERTY_8:
        BARDCODE_WRITE_PROPERTY( BardInt64 );
        continue;

      case BARDOP_WRITE_PROPERTY_X:
        {
          BardInt32 offset = BARDCODE_OPERAND_I32();
          BardInt32 size   = BARDCODE_OPERAND_I32();
          BardObject* context = BARD_POP_REF();
          BVM_NULL_CHECK( context, continue );
          memcpy( ((char*)context)+offset, bvm.regs.stack.data, size );
          bvm.regs.stack.data += size>>3;
        }
        continue;

      case BARDOP_WRITE_THIS_PROPERTY_REF:
        {
          BardObject* new_value = BARD_POP_REF();
          BardInt32 offset = BARDCODE_OPERAND_I32();
          BVM_DEREFERENCE(bvm.regs.frame.refs[-1],offset,BardObject*) = new_value;

          BardObject** dest = (BardObject**)(((char*)bvm.regs.frame.refs[-1])+offset);
          if (*dest) --((*dest)->reference_count);
          if (new_value) ++(new_value->reference_count);
          *dest = new_value;
        }
        continue;

      case BARDOP_WRITE_THIS_PROPERTY_4:
        BARDCODE_WRITE_THIS_PROPERTY( BardInt32 );
        continue;

      case BARDOP_WRITE_THIS_PROPERTY_REAL32:
        BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),BardReal32) = (BardReal32) BARD_POP_REAL();
        continue;

      case BARDOP_WRITE_THIS_PROPERTY_8:
        BARDCODE_WRITE_THIS_PROPERTY( BardInt64 );
        continue;

      case BARDOP_WRITE_THIS_PROPERTY_X:
        {
          BardInt32 offset = BARDCODE_OPERAND_I32();
          BardInt32 size   = BARDCODE_OPERAND_I32();
          memcpy( ((char*)bvm.regs.frame.refs[-1])+offset, bvm.regs.stack.data, size );
          bvm.regs.stack.data += size>>3;
        }
        continue;

      case BARDOP_WRITE_LOCAL_REF:
        bvm.regs.frame.refs[BARDCODE_OPERAND_I32()] = BARD_POP_REF();
        continue;

      case BARDOP_WRITE_LOCAL_8:
        bvm.regs.frame.data[BARDCODE_OPERAND_I32()] = BARD_POP_INTEGER();
        continue;

      case BARDOP_WRITE_LOCAL_X:
        {
          BardInt32 offset = BARDCODE_OPERAND_I32();
          BardInt32 slots  = BARDCODE_OPERAND_I32();
          memcpy( bvm.regs.frame.data+offset, bvm.regs.stack.data, slots<<3 );
          bvm.regs.stack.data += slots;
        }
        continue;

      case BARDOP_FAUX_STATIC_CALL:
        // Args have been evaluated & pushed on the stack but we don't
        // actually make the call since it's an empty method - we just
        // remove all the stuff on the stack again.
        m = bvm.methods[BARDCODE_OPERAND_I32()];
        bvm.regs.stack.refs  += m->parameters->num_ref_params;
        bvm.regs.stack.data += m->parameters->num_data_params;
        continue;

      case BARDOP_STATIC_CALL:
        m = bvm.methods[BARDCODE_OPERAND_I32()];
        BVM_NULL_CHECK( bvm.regs.stack.refs[BARDCODE_OPERAND_I32()], continue );
        goto execute_call;

      case BARDOP_DYNAMIC_CALL:
        {
          BardObject* context = bvm.regs.stack.refs[ BARDCODE_OPERAND_I32() ];
          if (context) 
          {
            m = context->type->dispatch_table[ BARDCODE_OPERAND_I32() ];
            goto execute_call;
          }
          else
          {
            BVM_THROW_TYPE( bvm.type_null_reference_error, continue );
            continue;
          }
        }

      case BARDOP_NEW_OBJECT:
        {
          BardTypeInfo* of_type = bvm.types[ BARDCODE_OPERAND_I32() ];
          BardObject* object = mm.create_object( of_type, of_type->object_size );
          BARD_PUSH_REF( object ); // first ref is result value
          BARD_PUSH_REF( object ); // second ref is for init() call
          BARD_PUSH_REF( object ); // third ref is for init_object() call
          m = of_type->method_init_object;
          goto execute_call;
        }

      case BARDOP_NEW_OBJECT_NO_INIT:
        {
          BardTypeInfo* of_type = bvm.types[ BARDCODE_OPERAND_I32() ];
          BardObject* object = mm.create_object( of_type, of_type->object_size );
          BARD_PUSH_REF( object ); // first ref is result value
          // we skip init_object() call
          BARD_PUSH_REF( object ); // second ref is for following init() call
          continue;
        }

      case BARDOP_NEW_ARRAY:
        {
          BardTypeInfo* of_type = bvm.types[ BARDCODE_OPERAND_I32() ];
          BardInt32 size = (BardInt32) BARD_POP_INTEGER();
          if (size < 0) BVM_THROW_TYPE( bvm.type_invalid_operand_error, continue );
          BARD_PUSH_REF( mm.create_array(of_type,size) );
          continue;
        }

      case BARDOP_ARRAY_READ_REF:
        {
          BardArray* array_ref = (BardArray*) BARD_POP_REF();
          BardInt32   index = (BardInt32) BARD_POP_INTEGER();
          if (!array_ref) BVM_THROW_TYPE(bvm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            BVM_THROW_TYPE( bvm.type_out_of_bounds_error, continue );
          }
          BARD_PUSH_REF( ((BardArray**)array_ref->data)[index] );
          continue;
        }

      case BARDOP_ARRAY_READ_1U:
        {
          BardArray* array_ref = (BardArray*) BARD_POP_REF();
          BardInt32   index = (BardInt32) BARD_POP_INTEGER();
          if (!array_ref) BVM_THROW_TYPE(bvm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            BVM_THROW_TYPE( bvm.type_out_of_bounds_error, continue );
          }
          BARD_PUSH_INTEGER( ((BardByte*)array_ref->data)[index] );
          continue;
        }

      case BARDOP_ARRAY_READ_2U:
        {
          BardArray* array_ref = (BardArray*) BARD_POP_REF();
          BardInt32   index = (BardInt32) BARD_POP_INTEGER();
          if (!array_ref) BVM_THROW_TYPE(bvm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            BVM_THROW_TYPE( bvm.type_out_of_bounds_error, continue );
          }
          BARD_PUSH_INTEGER( ((BardChar*)array_ref->data)[index] );
          continue;
        }

      case BARDOP_ARRAY_READ_4:
        {
          BardArray* array_ref = (BardArray*) BARD_POP_REF();
          BardInt32   index = (BardInt32) BARD_POP_INTEGER();
          if (!array_ref) BVM_THROW_TYPE(bvm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            BVM_THROW_TYPE( bvm.type_out_of_bounds_error, continue );
          }
          BARD_PUSH_INTEGER( ((BardInt32*)array_ref->data)[index] );
          continue;
        }

      case BARDOP_ARRAY_READ_REAL32:
        {
          BardArray* array_ref = (BardArray*) BARD_POP_REF();
          BardInt32      index = (BardInt32) BARD_POP_INTEGER();
          if (!array_ref) BVM_THROW_TYPE(bvm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            BVM_THROW_TYPE( bvm.type_out_of_bounds_error, continue );
          }
          BARD_PUSH_REAL( ((BardReal32*)array_ref->data)[index] );
          continue;
        }

      case BARDOP_ARRAY_READ_8:
        {
          BardArray* array_ref = (BardArray*) BARD_POP_REF();
          BardInt32   index = (BardInt32) BARD_POP_INTEGER();
          if (!array_ref) BVM_THROW_TYPE(bvm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            BVM_THROW_TYPE( bvm.type_out_of_bounds_error, continue );
          }
          BARD_PUSH_INTEGER( ((BardInt64*)array_ref->data)[index] );
          continue;
        }

      case BARDOP_ARRAY_READ_X:
        {
          BardArray* array_ref = (BardArray*) BARD_POP_REF();
          BardInt32 index = (BardInt32) BARD_POP_INTEGER();
          BVM_NULL_CHECK( array_ref, continue );
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            BVM_THROW_TYPE( bvm.type_out_of_bounds_error, continue );
          }
          int size = array_ref->type->element_size;
          memcpy( (bvm.regs.stack.data -= size>>3), ((char*)array_ref->data)+index*size, size );
          continue;
        }

      case BARDOP_ARRAY_WRITE_REF:
        {
          BardObject* new_value = BARD_POP_REF();
          BardArray* array_ref = (BardArray*) BARD_POP_REF();
          BardInt32   index = (BardInt32) BARD_POP_INTEGER();
          if (!array_ref) BVM_THROW_TYPE(bvm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            BVM_THROW_TYPE( bvm.type_out_of_bounds_error,continue );
          }
          BardObject** dest = ((BardObject**)array_ref->data) + index;
          if (*dest) --((*dest)->reference_count);
          if (new_value) ++new_value->reference_count;
          *dest = new_value;
          continue;
        }

      case BARDOP_ARRAY_WRITE_1:
        {
          BardInt64   new_value = BARD_POP_INTEGER();
          BardArray* array_ref = (BardArray*) BARD_POP_REF();
          BardInt32   index = (BardInt32) BARD_POP_INTEGER();
          if (!array_ref) BVM_THROW_TYPE(bvm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            BVM_THROW_TYPE( bvm.type_out_of_bounds_error,continue );
          }
          ((BardByte*)array_ref->data)[index] = (BardByte) new_value;
          continue;
        }

      case BARDOP_ARRAY_WRITE_2:
        {
          BardInt64   new_value = BARD_POP_INTEGER();
          BardArray* array_ref = (BardArray*) BARD_POP_REF();
          BardInt32   index = (BardInt32) BARD_POP_INTEGER();
          if (!array_ref) BVM_THROW_TYPE(bvm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            BVM_THROW_TYPE( bvm.type_out_of_bounds_error, continue );
          }
          ((BardChar*)array_ref->data)[index] = (BardChar) new_value;
          continue;
        }

      case BARDOP_ARRAY_WRITE_4:
        {
          BardInt64   new_value = BARD_POP_INTEGER();
          BardArray* array_ref = (BardArray*) BARD_POP_REF();
          BardInt32   index = (BardInt32) BARD_POP_INTEGER();
          if (!array_ref) BVM_THROW_TYPE(bvm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            BVM_THROW_TYPE( bvm.type_out_of_bounds_error, continue );
          }
          ((BardInt32*)array_ref->data)[index] = (BardInt32) new_value;
          continue;
        }

      case BARDOP_ARRAY_WRITE_REAL32:
        {
          BardReal64 new_value = BARD_POP_REAL();
          BardArray* array_ref = (BardArray*) BARD_POP_REF();
          BardInt32   index = (BardInt32) BARD_POP_INTEGER();
          if (!array_ref) BVM_THROW_TYPE(bvm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            BVM_THROW_TYPE( bvm.type_out_of_bounds_error, continue );
          }
          ((BardReal32*)array_ref->data)[index] = (BardReal32) new_value;
          continue;
        }

      case BARDOP_ARRAY_WRITE_8:
        {
          BardInt64   new_value = BARD_POP_INTEGER();
          BardArray* array_ref = (BardArray*) BARD_POP_REF();
          BardInt32   index = (BardInt32) BARD_POP_INTEGER();
          if (!array_ref) BVM_THROW_TYPE(bvm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            BVM_THROW_TYPE( bvm.type_out_of_bounds_error, continue );
          }
          ((BardInt64*)array_ref->data)[index] = (BardInt64) new_value;
          continue;
        }

      case BARDOP_ARRAY_WRITE_X:
        {
          BardArray* array_ref = (BardArray*) BARD_POP_REF();
          BVM_NULL_CHECK( array_ref, continue );
          int size = array_ref->type->element_size;
          char* source_ptr = (char*) bvm.regs.stack.data;
          bvm.regs.stack.data += size >> 3;
          BardInt32 index = (BardInt32) BARD_POP_INTEGER();
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            BVM_THROW_TYPE( bvm.type_out_of_bounds_error, continue );
          }
          memcpy( ((char*)array_ref->data)+index*size, source_ptr, size );
          continue;
        }

      case BARDOP_ARRAY_DUPLICATE:
        {
          BardObject* array = BARD_POP_REF();
          BARD_PUSH_REF( bard_duplicate_array(array) );
          continue;
        }

      case BARDOP_TYPECHECK:
        {
          BardTypeInfo* of_type = bvm.types[ BARDCODE_OPERAND_I32() ];
          BardObject* object = BARD_PEEK_REF();
          if (object && !object->type->instance_of(of_type)) 
          {
            BVM_THROW_TYPE( bvm.type_type_cast_error, continue );
          }
          continue;
        }

      case BARDOP_AS_REF:
        {
          BardTypeInfo* of_type = bvm.types[ BARDCODE_OPERAND_I32() ];
          BardObject* object = BARD_PEEK_REF();
          if (object && !object->type->instance_of(of_type)) BARD_PEEK_REF() = NULL;
          continue;
        }

      case BARDOP_CAST_REAL_TO_INTEGER:
        {
          BardReal64 value = *((BardReal64*)bvm.regs.stack.data);
          *bvm.regs.stack.data = (BardInt64) value;
        }
        continue;

      case BARDOP_CAST_INTEGER_TO_REAL:
        {
          BardInt64 value = *bvm.regs.stack.data;
          *((BardReal64*)bvm.regs.stack.data) = (BardReal64) value;
        }
        continue;

      case BARDOP_CAST_REAL_TO_LOGICAL:
        {
          BardReal64 value = *((BardReal64*)bvm.regs.stack.data);
          if (value != 0) *bvm.regs.stack.data = 1;
          else            *bvm.regs.stack.data = 0;
        }
        continue;

      case BARDOP_CAST_INTEGER_TO_LOGICAL:
        {
          BardInt64 value = *bvm.regs.stack.data;
          if (value != 0) *bvm.regs.stack.data = 1;
          else            *bvm.regs.stack.data = 0;
        }
        continue;

      case BARDOP_CAST_INTEGER_TO_I32:
        *bvm.regs.stack.data = (BardInt32) *bvm.regs.stack.data;
        continue;

      case BARDOP_CAST_INTEGER_TO_CHAR:
        *bvm.regs.stack.data &= 0xffff;
        continue;

      case BARDOP_CAST_INTEGER_TO_BYTE:
        *bvm.regs.stack.data &= 0xff;
        continue;

      case BARDOP_CMP_INSTANCE_OF:
        {
          BardTypeInfo* of_type = bvm.types[ BARDCODE_OPERAND_I32() ];
          BardObject* object = BARD_POP_REF();
          if (object) BARD_PUSH_INTEGER( object->type->instance_of(of_type));
          else BARD_PUSH_INTEGER( 0 );
        }
        continue;

      case BARDOP_CMP_EQ_REF:
        {
          BardObject* b = BARD_POP_REF();
          BardObject* a = BARD_POP_REF();
          BARD_PUSH_INTEGER( a == b);
        }
        continue;

      case BARDOP_CMP_NE_REF:
        {
          BardObject* b = BARD_POP_REF();
          BardObject* a = BARD_POP_REF();
          BARD_PUSH_INTEGER( a != b);
        }
        continue;

      case BARDOP_CMP_EQ_REF_NULL:
        {
          BardObject* obj = BARD_POP_REF();
          BARD_PUSH_INTEGER( obj == NULL );
        }
        continue;

      case BARDOP_CMP_NE_REF_NULL:
        {
          BardObject* obj = BARD_POP_REF();
          BARD_PUSH_INTEGER( obj != NULL );
        }
        continue;

      case BARDOP_CMP_EQ_X:
        {
          BardInt32 result = 1;
          BardInt32 slots = BARDCODE_OPERAND_I32();
          
          BardInt64* ptr1 = (BardInt64*) bvm.regs.stack.data;
          bvm.regs.stack.data += slots;
          BardInt64* ptr2 = (BardInt64*) bvm.regs.stack.data;
          bvm.regs.stack.data += slots;

          ++slots;
          --ptr1;
          --ptr2;
          while (--slots)
          {
            if (*(++ptr1) != *(++ptr2))
            {
              result = 0;
              break;
            }
          }

          BARD_PUSH_INTEGER( result );
        }
        continue;

      case BARDOP_CMP_NE_X:
        {
          BardInt32 result = 0;
          BardInt32 slots = BARDCODE_OPERAND_I32();
          
          BardInt64* ptr1 = (BardInt64*) bvm.regs.stack.data;
          bvm.regs.stack.data += slots;
          BardInt64* ptr2 = (BardInt64*) bvm.regs.stack.data;
          bvm.regs.stack.data += slots;

          ++slots;
          --ptr1;
          --ptr2;
          while (--slots)
          {
            if (*(++ptr1) != *(++ptr2))
            {
              result = 1;
              break;
            }
          }

          BARD_PUSH_INTEGER( result );
        }
        continue;

      case BARDOP_CMP_EQ_INTEGER:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a == b);
        }
        continue;

      case BARDOP_CMP_NE_INTEGER:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a != b);
        }
        continue;

      case BARDOP_CMP_GT_INTEGER:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a > b);
        }
        continue;

      case BARDOP_CMP_GE_INTEGER:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a >= b);
        }
        continue;

      case BARDOP_CMP_LT_INTEGER:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a < b);
        }
        continue;

      case BARDOP_CMP_LE_INTEGER:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a <= b);
        }
        continue;

      case BARDOP_CMP_EQ_REAL:
        {
          BardReal64 b = BARD_POP_REAL();
          BardReal64 a = BARD_PEEK_REAL();
          BARD_PEEK_INTEGER() = (a == b);
        }
        continue;

      case BARDOP_CMP_NE_REAL:
        {
          BardReal64 b = BARD_POP_REAL();
          BardReal64 a = BARD_PEEK_REAL();
          BARD_PEEK_INTEGER() = (a != b);
        }
        continue;

      case BARDOP_CMP_GT_REAL:
        {
          BardReal64 b = BARD_POP_REAL();
          BardReal64 a = BARD_PEEK_REAL();
          BARD_PEEK_INTEGER() = (a > b);
        }
        continue;

      case BARDOP_CMP_GE_REAL:
        {
          BardReal64 b = BARD_POP_REAL();
          BardReal64 a = BARD_PEEK_REAL();
          BARD_PEEK_INTEGER() = (a >= b);
        }
        continue;

      case BARDOP_CMP_LT_REAL:
        {
          BardReal64 b = BARD_POP_REAL();
          BardReal64 a = BARD_PEEK_REAL();
          BARD_PEEK_INTEGER() = (a < b);
        }
        continue;

      case BARDOP_CMP_LE_REAL:
        {
          BardReal64 b = BARD_POP_REAL();
          BardReal64 a = BARD_PEEK_REAL();
          BARD_PEEK_INTEGER() = (a <= b);
        }
        continue;

      case BARDOP_NOT_INTEGER:
        *bvm.regs.stack.data = ~(*bvm.regs.stack.data);
        continue;

      case BARDOP_NOT_LOGICAL:
        // Logical values can be -1, 0, or 1, so a little extra attention is required.
        if (*bvm.regs.stack.data > 0) *bvm.regs.stack.data = 0;
        else                          *bvm.regs.stack.data = 1;
        continue;

      case BARDOP_NEGATE_INTEGER:
        *bvm.regs.stack.data = -(*bvm.regs.stack.data);
        continue;

      case BARDOP_NEGATE_REAL:
        *((BardReal64*)bvm.regs.stack.data) = -(*((BardReal64*)bvm.regs.stack.data));
        continue;

      case BARDOP_AND_LOGICAL:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a & b);
        }
        continue;

      case BARDOP_OR_LOGICAL:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a | b);
        }
        continue;

      case BARDOP_XOR_LOGICAL:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a ^ b);
        }
        continue;

      case BARDOP_ADD_INTEGER:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a + b);
        }
        continue;

      case BARDOP_SUB_INTEGER:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a - b);
        }
        continue;

      case BARDOP_MUL_INTEGER:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a * b);
        }
        continue;

      case BARDOP_DIV_INTEGER:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BVM_ASSERT( b, bvm.type_divide_by_zero_error, continue );
          BARD_PEEK_INTEGER() = (a / b);
        }
        continue;

      case BARDOP_MOD_INTEGER:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BVM_ASSERT( b, bvm.type_divide_by_zero_error, continue );
          if (b == 1)
          {
            BARD_PEEK_INTEGER() = 0;
          }
          else if ((a ^ b) < 0)
          {
            BardInt64 r = a % b;
            BARD_PEEK_INTEGER() = (r ? (r+b) : r);
          }
          else
          {
            BARD_PEEK_INTEGER() = (a % b);
          }
        }
        continue;

      case BARDOP_EXP_INTEGER:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (BardInt64) pow((double)a,(double)b);
        }
        continue;

      case BARDOP_AND_INTEGER:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a & b);
        }
        continue;

      case BARDOP_OR_INTEGER:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a | b);
        }
        continue;

      case BARDOP_XOR_INT64:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a ^ b);
        }
        continue;

      case BARDOP_SHL_INT64:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a << b);
        }
        continue;

      case BARDOP_SHR_INT64:
        {
          BardInt64 b = BARD_POP_INTEGER();
          if (b == 0) continue;

          BardInt64 a = BARD_PEEK_INTEGER();
          a = (a >> 1) & 0x7fffFFFFffffFFFFLL;
          if (--b > 0) a >>= b;

          BARD_PEEK_INTEGER() = a;
        }
        continue;

      case BARDOP_SHRX_INT64:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a >> b);
        }
        continue;

      case BARDOP_XOR_INT32:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (BardInt32)(a ^ b);
        }
        continue;

      case BARDOP_SHL_INT32:
        {
          BardInt64 b = BARD_POP_INTEGER();
          BardInt64 a = BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (BardInt32)(a << b);
        }
        continue;

      case BARDOP_SHR_INT32:
        {
          BardInt64 b = BARD_POP_INTEGER();
          if (b == 0) continue;

          BardInt64 a = BARD_PEEK_INTEGER();
          a = (a >> 1) & 0x7fffFFFF;
          if (--b > 0) a >>= b;
          BARD_PEEK_INTEGER() = a;
        }
        continue;

      case BARDOP_SHRX_INT32:
        {
          BardInt32 b = (BardInt32) BARD_POP_INTEGER();
          BardInt32 a = (BardInt32) BARD_PEEK_INTEGER();
          BARD_PEEK_INTEGER() = (a >> b);
        }
        continue;

      case BARDOP_ADD_REAL:
        {
          BardReal64 b = BARD_POP_REAL();
          BardReal64 a = BARD_PEEK_REAL();
          BARD_PEEK_REAL() = (a + b);
        }
        continue;

      case BARDOP_SUB_REAL:
        {
          BardReal64 b = BARD_POP_REAL();
          BardReal64 a = BARD_PEEK_REAL();
          BARD_PEEK_REAL() = (a - b);
        }
        continue;

      case BARDOP_MUL_REAL:
        {
          BardReal64 b = BARD_POP_REAL();
          BardReal64 a = BARD_PEEK_REAL();
          BARD_PEEK_REAL() = (a * b);
        }
        continue;

      case BARDOP_DIV_REAL:
        {
          BardReal64 b = BARD_POP_REAL();
          BardReal64 a = BARD_PEEK_REAL();
          BARD_PEEK_REAL() = (a / b);
        }
        continue;

      case BARDOP_MOD_REAL:
        {
          BardReal64 b = BARD_POP_REAL();
          BardReal64 a = BARD_PEEK_REAL();
          BardReal64 q = a / b;
          BARD_PEEK_REAL() = (BardReal64) (a - (floor(q)) * b);
        }
        continue;

      case BARDOP_EXP_REAL:
        {
          BardReal64 b = BARD_POP_REAL();
          BardReal64 a = BARD_PEEK_REAL();
          BARD_PEEK_REAL() = pow(a,b);
        }
        continue;

      case BARDOP_THIS_ADD_ASSIGN_I32:
        BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),BardInt32) += (BardInt32) BARD_POP_INTEGER();
        continue;

      case BARDOP_THIS_SUB_ASSIGN_I32:
        BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),BardInt32) -= (BardInt32) BARD_POP_INTEGER();
        continue;

      case BARDOP_THIS_MUL_ASSIGN_I32:
        BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),BardInt32) *= (BardInt32) BARD_POP_INTEGER();
        continue;

      case BARDOP_THIS_INCREMENT_I32:
        ++BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),BardInt32);
        continue;

      case BARDOP_THIS_DECREMENT_I32:
        --BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),BardInt32);
        continue;

      case BARDOP_THIS_ADD_ASSIGN_R64:
        BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),BardReal64) += BARD_POP_REAL();
        continue;

      case BARDOP_THIS_SUB_ASSIGN_R64:
        BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),BardReal64) -= BARD_POP_REAL();
        continue;

      case BARDOP_THIS_MUL_ASSIGN_R64:
        BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),BardReal64) *= BARD_POP_REAL();
        continue;

      case BARDOP_THIS_INCREMENT_R64:
        ++BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),BardReal64);
        continue;

      case BARDOP_THIS_DECREMENT_R64:
        --BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),BardReal64);
        continue;

      case BARDOP_LOCAL_ADD_ASSIGN_INTEGER:
        bvm.regs.frame.data[BARDCODE_OPERAND_I32()] += BARD_POP_INTEGER();
        continue;

      case BARDOP_LOCAL_SUB_ASSIGN_INTEGER:
        bvm.regs.frame.data[BARDCODE_OPERAND_I32()] -= BARD_POP_INTEGER();
        continue;

      case BARDOP_LOCAL_MUL_ASSIGN_INTEGER:
        bvm.regs.frame.data[BARDCODE_OPERAND_I32()] *= BARD_POP_INTEGER();
        continue;

      case BARDOP_LOCAL_INCREMENT_INTEGER:
        ++bvm.regs.frame.data[BARDCODE_OPERAND_I32()];
        continue;

      case BARDOP_LOCAL_DECREMENT_INTEGER:
        --bvm.regs.frame.data[BARDCODE_OPERAND_I32()];
        continue;

      case BARDOP_LOCAL_ADD_ASSIGN_REAL:
        ((BardReal64*)bvm.regs.frame.data)[BARDCODE_OPERAND_I32()] += BARD_POP_REAL();
        continue;

      case BARDOP_LOCAL_SUB_ASSIGN_REAL:
        ((BardReal64*)bvm.regs.frame.data)[BARDCODE_OPERAND_I32()] -= BARD_POP_REAL();
        continue;

      case BARDOP_LOCAL_MUL_ASSIGN_REAL:
        ((BardReal64*)bvm.regs.frame.data)[BARDCODE_OPERAND_I32()] *= BARD_POP_REAL();
        continue;

      case BARDOP_LOCAL_INCREMENT_REAL:
        ++((BardReal64*)bvm.regs.frame.data)[BARDCODE_OPERAND_I32()];
        continue;

      case BARDOP_LOCAL_DECREMENT_REAL:
        --((BardReal64*)bvm.regs.frame.data)[BARDCODE_OPERAND_I32()];
        continue;

      default:
        {
          char buffer[80];
          sprintf( buffer, "Unhandled opcode: %d", *(--bvm.regs.ip) );
          bard_throw_fatal_error( buffer );
        }
    }

    printf( "ERROR\n" );

    execute_call:
      // Calls method referenced by 'm'.
      if (bvm.regs.call_frame < bvm.min_call_stack)
      {
        throw_stack_limit_error();
      }
      // Save prior call frame.
      (--bvm.regs.call_frame)->called_method    = m;
      bvm.regs.call_frame->prior_frame          = bvm.regs.frame;
      bvm.regs.call_frame->return_address       = bvm.regs.ip;

      // Set up current call frame pointers to be just before parameter data.
      bvm.regs.frame.refs  = bvm.regs.stack.refs  + m->parameters->num_ref_params;
      bvm.regs.frame.data = bvm.regs.stack.data + m->parameters->num_data_params;

      if (bvm.regs.stack.data < bvm.min_data_stack || bvm.regs.stack.refs < bvm.min_ref_stack)
      {
        throw_stack_limit_error();
      }

      // Adjust stack pointers to make room for local data & clear that region.
      {
        int count = m->num_local_refs;
        if (count)
        {
          bvm.regs.stack.refs -= count;

          // Need to clear this in case a GC happens before all the locals
          // are initialized.
          memset( bvm.regs.stack.refs, 0, count*sizeof(BardObject*) );
        }

        bvm.regs.stack.data -= m->local_data_size;
      }
      bvm.regs.ip = m->bytecode;
      continue;
  }
}

static BardCatchInfo* find_catch( BardObject* err, int ip_offset )
{
  // Returns a compatible catch in the current method or NULL if not found.
  BardMethodInfo* m = bvm.regs.call_frame->called_method;
  for (int i=0; i<m->catches.count; ++i)
  {
    BardCatchInfo* cur_catch = m->catches[i];
    if (ip_offset >= cur_catch->begin_offset && ip_offset <= cur_catch->end_offset)
    {
      if (err->type->instance_of(cur_catch->type_caught)) return cur_catch;
    }
  }
  return NULL;
}


static BardCatchInfo* unwind_stack( BardObject* err )
{
  // Unwind call frames until a suitable 'catch' is found.
  BardCatchInfo* catch_info = NULL;
  while (bvm.regs.call_frame < bvm.call_stack_limit)
  {
    int ip_offset = ((BardOpcode*)(void*)bvm.regs.ip) - bvm.code.data;
    catch_info = find_catch(err,ip_offset);
    if (catch_info) break;
    bvm.regs.stack = bvm.regs.frame;
    bvm.regs.frame = bvm.regs.call_frame->prior_frame;
    bvm.regs.ip = (bvm.regs.call_frame++)->return_address;
  }
  return catch_info;
}

static void activate_catch( BardCatchInfo* catch_info, BardObject* err )
{
  if ( !catch_info )
  {
    // Do a to_String on the error, save it as the global error message,
    // and abort the program.
    BardMethodInfo* m_to_string = err->type->must_find_method("to_String()");
    BARD_PUSH_REF( err );
    bvm.call( m_to_string );
    BardString* mesg = (BardString*) BARD_POP_REF();
    bard_error_message = mesg->to_new_ascii();
#if BARD_USE_LONGJMP
    longjmp( bard_fatal_jumppoint, 1 );
#else
    throw 1;
#endif
  }

  bvm.regs.ip = bvm.code.data + catch_info->handler;
  BARD_PUSH_REF( err );
}

void BardVM::throw_exception_on_stack()
{
  // Pop off exception object
  BardObject* err = BARD_POP_REF();

  // Handle null exception reference
  if ( !err )
  {
    bard_throw_fatal_error( "Thrown exception is null." );
  }

  activate_catch( unwind_stack(err), err );
}

void BardVM::throw_exception( BardTypeInfo* type, const char* mesg )
{
  exception_ip = regs.ip;

  BardObject* err = type->create();
  BARD_PUSH_REF( err ); // result on stack
  BARD_PUSH_REF( err ); // for init() / init(String)
  BARD_PUSH_REF( err ); // for init_object()

  call( type->method_init_object );

  if (mesg)
  {
    BARD_PUSH_REF( (BardObject*) BardString::create(mesg) );
    call( type->must_find_method("init(String)") );
  }
  else
  {
    call( type->must_find_method("init()") );
  }

  throw_exception_on_stack();
}

void BardVM::throw_stack_limit_error()
{
  // Stack limit errors are a little different since we need to
  // save the history, create the error object, unwind the stack, 
  // and init the error object.
  exception_ip = regs.ip;

  ArrayList<BardInt64> history;
  get_call_history(history);

  BardTypeInfo* type = bvm.type_stack_limit_error;
  BardObject* err = type->create();

  BardCatchInfo* catch_info = unwind_stack(err);

  // initialize the object now that we've got some breathing room
  BARD_PUSH_REF( err );
  BardArray* array = bvm.type_array_of_int64->create( history.count );
  memcpy( array->data, history.data, history.count * 8 );
  BARD_PUSH_REF( array );
  call( type->must_find_method("init(Array<<Int64>>)") );

  activate_catch( catch_info, err );
}

//=============================================================================
//  Loader
//=============================================================================
#define BARDCMD_SET_FILENAME_AND_LINE      0
#define BARDCMD_SET_LINE                   1
#define BARDCMD_INC_LINE                   2
#define BARDCMD_STRUCTURE_ID               3
#define BARDCMD_RETURN_VALUE               4
#define BARDCMD_RETURN_NIL                 5
#define BARDCMD_BLOCK                      6
#define BARDCMD_TRYCATCH                   7
#define BARDCMD_THROW                      8
#define BARDCMD_IF                         9
#define BARDCMD_CONTINGENT                10
#define BARDCMD_NECESSARY                 11
#define BARDCMD_SUFFICIENT                12
#define BARDCMD_WHILE                     13
#define BARDCMD_LOOP                      14
#define BARDCMD_ESCAPE                    15
#define BARDCMD_NEXT_ITERATION            16
#define BARDCMD_BREAKPOINT                17
#define BARDCMD_LITERAL_STRING            18
#define BARDCMD_LITERAL_INT64             19
#define BARDCMD_LITERAL_INT32             20
#define BARDCMD_LITERAL_CHAR              21
#define BARDCMD_LITERAL_BYTE              22
#define BARDCMD_LITERAL_REAL64            23
#define BARDCMD_LITERAL_REAL32            24
#define BARDCMD_LITERAL_LOGICAL_TRUE      25
#define BARDCMD_LITERAL_LOGICAL_FALSE     26
#define BARDCMD_LITERAL_LOGICAL_VOID      27
#define BARDCMD_LITERAL_NULL              28
#define BARDCMD_THIS_REFERENCE            29
#define BARDCMD_SINGLETON_REFERENCE       30
#define BARDCMD_SINGLETON_WRITE           31
#define BARDCMD_PROPERTY_READ             32
#define BARDCMD_COMPOUND_PROPERTY_READ    33
#define BARDCMD_LOCAL_VAR_READ            34
#define BARDCMD_PROPERTY_WRITE            35
#define BARDCMD_LOCAL_VAR_WRITE           36
#define BARDCMD_STATIC_CALL               37
#define BARDCMD_DYNAMIC_CALL              38
#define BARDCMD_NEW_OBJECT                39
#define BARDCMD_NEW_COMPOUND              40
#define BARDCMD_ARRAY_CREATE              41
#define BARDCMD_ARRAY_GET                 42
#define BARDCMD_ARRAY_SET                 43
#define BARDCMD_NARROWING_CAST            44
#define BARDCMD_WIDENING_CAST             45
#define BARDCMD_CAST_TO_INT64             46
#define BARDCMD_CAST_TO_INT32             47
#define BARDCMD_CAST_TO_CHAR              48
#define BARDCMD_CAST_TO_BYTE              49
#define BARDCMD_CAST_TO_REAL64            50
#define BARDCMD_CAST_TO_REAL32            51
#define BARDCMD_CAST_TO_LOGICAL           52
#define BARDCMD_AS                        53
#define BARDCMD_COERCE_AS                 54
#define BARDCMD_CMP_INSTANCE_OF           55
#define BARDCMD_CMP_IS                    56
#define BARDCMD_CMP_IS_NOT                57
#define BARDCMD_CMP_IS_NULL               58
#define BARDCMD_CMP_IS_NOT_NULL           59
#define BARDCMD_CMP_EQ                    60
#define BARDCMD_CMP_NE                    61
#define BARDCMD_CMP_LE                    62
#define BARDCMD_CMP_LT                    63
#define BARDCMD_CMP_GT                    64
#define BARDCMD_CMP_GE                    65
#define BARDCMD_NOT                       66
#define BARDCMD_NEGATE                    67
#define BARDCMD_ADD                       68
#define BARDCMD_SUB                       69
#define BARDCMD_MUL                       70
#define BARDCMD_DIV                       71
#define BARDCMD_MOD                       72
#define BARDCMD_EXP                       73
#define BARDCMD_AND                       74
#define BARDCMD_OR                        75
#define BARDCMD_XOR                       76
#define BARDCMD_LAZY_AND                  77
#define BARDCMD_LAZY_OR                   78
#define BARDCMD_LEFT_SHIFTED              79
#define BARDCMD_RIGHT_SHIFTED             80
#define BARDCMD_RIGHT_XSHIFTED            81
#define BARDCMD_THIS_ADD_AND_ASSIGN       82
#define BARDCMD_THIS_SUB_AND_ASSIGN       83
#define BARDCMD_THIS_MUL_AND_ASSIGN       84
#define BARDCMD_THIS_DIV_AND_ASSIGN       85
#define BARDCMD_THIS_MOD_AND_ASSIGN       86
#define BARDCMD_LOCAL_VAR_ADD_AND_ASSIGN  87
#define BARDCMD_LOCAL_VAR_SUB_AND_ASSIGN  88
#define BARDCMD_LOCAL_VAR_MUL_AND_ASSIGN  89
#define BARDCMD_LOCAL_VAR_DIV_AND_ASSIGN  90
#define BARDCMD_LOCAL_VAR_MOD_AND_ASSIGN  91
#define BARDCMD_THIS_INCREMENT            92
#define BARDCMD_THIS_DECREMENT            93
#define BARDCMD_LOCAL_INCREMENT           94
#define BARDCMD_LOCAL_DECREMENT           95
#define BARDCMD_ARRAY_DUPLICATE           96

void BardVM_unhandled_native_method()
{
  BardMethodInfo* m = bvm.regs.call_frame->called_method;
  bard_throw_fatal_error( "Unhandled native call to ", m->type_context->name, 
      "::", m->signature, "." );
}


//=============================================================================
//  BardReader
//=============================================================================
BardReader::~BardReader()
{
  if (free_data && data) delete data;
  data = 0;
  remaining = 0;
  pos = 0;
}

void BardReader::init( const char* original_filename )
{
  int len = strlen(original_filename);
  if (len > 250) bard_throw_fatal_error( "Filename too long." );

  char filename[256];
  strcpy( filename, original_filename );

  FILE* fp = fopen( filename, "rb" );
  if ( !fp )
  {
    if (len > 5)
    {
      if (0 == strcmp( filename+(len-5), ".bard" ) )
      {
        len -= 5;
        filename[len] = 0;
      }
    }
    if (len < 5 || 0 != strcmp(filename+(len-4),".etc"))
    {
      strcat( filename, ".etc" );
      fp = fopen( filename, "rb" );
    }
    if ( !fp )
    {
      bard_throw_fatal_error( "Error opening file \"", filename, "\"." );
    }
  }

  fseek( fp, 0, SEEK_END );
  remaining = ftell(fp);
  total_size = remaining;
  fseek( fp, 0, SEEK_SET );

  data = new unsigned char[remaining];
  fread( (void*) data, 1, remaining, fp );
  fclose(fp);

  pos = 0;
  free_data = true;
}

void BardReader::init( const unsigned char* data, int size )
{
  this->data = data;
  pos = 0;
  remaining = size;
  free_data = false;
  total_size = remaining;
}

int BardReader::read()
{
  if (pos == remaining) bard_throw_fatal_error( "Unexpected EOF" );
  return data[pos++];
}

int BardReader::read16()
{
  if (pos+2 > remaining) bard_throw_fatal_error( "Unexpected EOF" );
  int result = data[pos++];
  result = (result << 8) | data[pos++];
  return result;
}

int BardReader::read32()
{
  if (pos+4 > remaining) bard_throw_fatal_error( "Unexpected EOF" );
  int result = data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  return result;
}

BardInt64 BardReader::read64()
{
  if (pos+8 > remaining) bard_throw_fatal_error( "Unexpected EOF" );
  BardInt64 result = data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  return result;
}

int BardReader::readX()
{
  // %0     xxxxxxx
  // %10    xxxxxxxxxxxxxx
  // %110   xxxxxxxxxxxxxxxxxxxxx
  // %1110  xxxxxxxxxxxxxxxxxxxxxxxxxxxx
  // %11110 000xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  //
  // All values have 1 subtracted from them before
  // returning - this makes the common value "-1"
  // a short representation.
  int b1 = read();

  // %0     xxxxxxx
  if ((b1 & 0x80) == 0) return b1-1;

  // %10    xxxxxxxxxxxxxx
  if ((b1 & 0x40) == 0)
  {
    int r = (b1 & ~0x80) << 8;;
    r |= read();
    return r-1;
  }

  // %110   xxxxxxxxxxxxxxxxxxxxx
  if ((b1 & 0x20) == 0)
  {
    int r = (b1 & 31) << 16;
    r |= read16();
    return r-1;
  }

  // %1110  xxxxxxxxxxxxxxxxxxxxxxxxxxxx
  if ((b1 & 0x10) == 0)
  {
    int r = (b1 & 15) << 24;
    r |= read16() << 8;
    r |= read();
    return r-1;
  }

  // %11110 000xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  return read32()-1;
}

int BardReader::read_utf8()
{
  int ch = read();

  if ((ch & 0x80) != 0)
  {
    int ch2 = read();

    if ((ch & 0x20) == 0)
    {
      // %110xxxxx 10xxxxxx
      ch  &= 0x1f;
      ch2 &= 0x3f;
      ch = (ch << 6) | ch2;
    }
    else
    {
      // %1110xxxx 10xxxxxx 10xxxxxx
      int ch3 = read();
      ch  &= 15;
      ch2 &= 0x3f;
      ch3 &= 0x3f;
      ch = (ch << 12) | (ch2 << 6) | ch3;
    }
  }

  return ch;
}

char* BardReader::read_new_ascii()
{
  int count = readX();
  char* result = new char[count+1];
  for (int i=0; i<count; ++i) result[i] = read();
  result[count] = 0;
  return result;
}

//=============================================================================
//  BardLoader
//=============================================================================
void BardLoader::load( const char* filename )
{
  reader.init( filename );
  load();
}

void BardLoader::load( const char* data, int count )
{
  reader.init( (const unsigned char*) data, count );
  load();
}

void BardLoader::load()
{
  load_version();
  load_filenames();
  load_identifiers();
  load_type_info();
  load_parameter_info();
  load_method_info();
  load_type_defs();
  load_string_table();
  load_method_defs();
}

void BardLoader::load_version()
{
  must_consume_header( "ETC" );
  reader.read32();  // discard version
}

void BardLoader::load_filenames()
{
  must_consume_header( "FILENAMES" );
  int n = reader.readX();
  bvm.filenames.ensure_capacity(n);
  for (int i=0; i<n; ++i) bvm.filenames.add( reader.read_new_ascii() );
}

void BardLoader::load_identifiers()
{
  must_consume_header( "IDENTIFIERS" );
  int n = reader.readX();
  bvm.identifiers.ensure_capacity(n);
  for (int i=0; i<n; ++i) bvm.identifiers.add( reader.read_new_ascii() );
}

void BardLoader::load_type_info()
{
  must_consume_header( "TYPEINFO" );
  int n = reader.readX();
  bvm.types.ensure_capacity(n);
  for (int i=0; i<n; ++i)
  {
    bvm.types.add( new BardTypeInfo() );
  }

  for (int i=0; i<n; ++i)
  {
    BardTypeInfo* type = bvm.types[i];
    type->name = load_id();
    type->index = i;
    type->qualifiers = reader.readX();

    int num_base_types = reader.readX();
    type->base_types.ensure_capacity(num_base_types);
    for (int j=0; j<num_base_types; ++j)
    {
      type->base_types.add( load_type() );
    }
    type->element_type = load_type();

  }
  bvm.main_class = load_type();

  bvm.type_object  = bvm.must_find_type( "Object" );
  bvm.type_int64   = bvm.must_find_type( "Int64" );
  bvm.type_int32   = bvm.must_find_type( "Int32" );
  bvm.type_char    = bvm.must_find_type( "Char" );
  bvm.type_byte    = bvm.must_find_type( "Byte" );
  bvm.type_real64  = bvm.must_find_type( "Real64" );
  bvm.type_real32  = bvm.must_find_type( "Real32" );
  bvm.type_logical = bvm.must_find_type( "Logical" );
  bvm.type_null    = bvm.must_find_type( "null" );
  bvm.type_string  = bvm.must_find_type( "String" );
  bvm.type_system  = bvm.must_find_type( "System" );
  bvm.type_native_data           = bvm.must_find_type( "NativeData" );
  bvm.type_weak_reference        = bvm.must_find_type( "WeakReference" );
  bvm.type_array_of_char         = bvm.must_find_type( "Array<<Char>>" );
  bvm.type_array_of_int64        = bvm.must_find_type( "Array<<Int64>>" );
  bvm.type_requires_cleanup      = bvm.must_find_type( "RequiresCleanup" );
  bvm.type_missing_return_error  = bvm.must_find_type( "MissingReturnError" );
  bvm.type_type_cast_error       = bvm.must_find_type( "TypeCastError" );
  bvm.type_out_of_bounds_error   = bvm.must_find_type( "OutOfBoundsError" );
  bvm.type_divide_by_zero_error  = bvm.must_find_type( "DivideByZeroError" );
  bvm.type_null_reference_error  = bvm.must_find_type( "NullReferenceError" );
  bvm.type_stack_limit_error     = bvm.must_find_type( "CallStackLimitReached" );
  bvm.type_file_error            = bvm.must_find_type( "FileError" );
  bvm.type_file_not_found_error  = bvm.must_find_type( "FileNotFoundError" );
  bvm.type_no_next_value_error   = bvm.must_find_type( "NoNextValueError" );
  bvm.type_socket_error          = bvm.must_find_type( "SocketError" );
  bvm.type_invalid_operand_error = bvm.must_find_type( "InvalidOperandError" );

}

void BardLoader::load_parameter_info()
{
  must_consume_header( "PARAMETERINFO" );
  int n = reader.readX();
  bvm.parameter_table.ensure_capacity(n);

  for (int i=0; i<n; ++i)
  {
    int num_params = reader.readX();
    BardParameterList* list = new BardParameterList(num_params);
    bvm.parameter_table.add( list );

    for (int i=0; i<num_params; ++i)
    {
      list->parameters.add( new BardLocalVarInfo(load_type()) );
    }
  }
}

void BardLoader::load_method_info()
{
  must_consume_header( "METHODINFO" );
  int n = reader.readX();
  bvm.methods.ensure_capacity(n);

  for (int i=0; i<n; ++i)
  {
    BardMethodInfo* m = new BardMethodInfo();
    bvm.methods.add(m);

    m->name = load_id();
    m->index = i;
    m->qualifiers = reader.readX();
    m->type_context = load_type();
    m->parameters = bvm.parameter_table[ reader.readX() ];

    int index = reader.readX();
    m->return_type = (index==-1) ? NULL : bvm.types[index];
    m->catches.ensure_capacity( reader.readX() );

    int num_locals = reader.readX();
    m->local_vars.ensure_capacity( num_locals );
    for (int v=0; v<num_locals; ++v)
    {
      m->local_vars.add( new BardLocalVarInfo(load_type()) );
    }

    if (m->qualifiers & BARD_QUALIFIER_NATIVE)
    {
      m->native_handler = (BardNativeFn) BardVM_unhandled_native_method;
    }
  }
}

void BardLoader::load_type_defs()
{
  must_consume_header( "TYPEDEFS" );

  for (int t=0; t<bvm.types.count; ++t)
  {
    BardTypeInfo* type = bvm.types[t];

    int n = reader.readX();
    type->properties.ensure_capacity( n );

    int num_reference_properties = 0;
    for (int i=0; i<n; ++i)
    {
      BardPropertyInfo* v = new BardPropertyInfo( load_type() );
      type->properties.add( v );
      v->name = load_id();
      if (v->type->is_reference()) ++num_reference_properties;
    }
    type->reference_property_offsets.ensure_capacity(num_reference_properties);

    n = reader.readX();
    type->methods.ensure_capacity(n);
    for (int i=0; i<n; ++i) type->methods.add( load_method() );

    n = reader.readX();
    type->dispatch_table.ensure_capacity(n);
    for (int i=0; i<n; ++i) type->dispatch_table.add( load_method() );
  }

  bvm.prep_types_and_methods();

  // Set array element sizes
  for (int i=0; i<bvm.types.count; ++i)
  {
    BardTypeInfo* type = bvm.types[i];
    BardTypeInfo* element_type = (BardTypeInfo*) type->element_type;
    if (element_type)
    {
      type->qualifiers |= BARD_QUALIFIER_ARRAY;
      if (element_type->is_reference())
      {
        type->element_size = sizeof(void*);
        type->qualifiers |= BARD_QUALIFIER_REFERENCE_ARRAY;
      }
      else if (element_type == bvm.type_int64)
      {
        type->element_size = 8;
      }
      else if (element_type == bvm.type_int32)
      {
        type->element_size = 4;
      }
      else if (element_type == bvm.type_char)
      {
        type->element_size = 2;
      }
      else if (element_type == bvm.type_byte)
      {
        type->element_size = 1;
      }
      else if (element_type == bvm.type_real64)
      {
        type->element_size = 8;
      }
      else if (element_type == bvm.type_real32)
      {
        type->element_size = 4;
      }
      else if (element_type == bvm.type_logical)
      {
        //type->element_size = 1;
        type->element_size = 4;
      }
      else
      {
        type->element_size = element_type->object_size;
      }
    }
  }

}

void BardLoader::load_string_table()
{
  must_consume_header( "STRINGTABLE" );

  int n = reader.readX();
  bvm.strings.ensure_capacity(n);

  for (int i=0; i<n; ++i)
  {
    int count = reader.readX();
    BardString* string = (BardString*) 
      (mm.create_object( bvm.type_string, (sizeof(BardString)-2) + count*2 ));
    ++string->reference_count;
    string->count = count;

    BardChar*   data = (BardChar*) string->characters;
    ++count;
    --data;
    while (--count) *(++data) = reader.read_utf8();

    string->set_hash_code();

    bvm.strings.add( string );
  }
}

void BardLoader::load_method_defs()
{
  must_consume_header( "METHODDEFS" );

  bvm.code.ensure_capacity( reader.total_size / 8 ); // seems to be a decent metric
  bvm.line_table.ensure_capacity( reader.total_size / 16 );

  for (int i=0; i<bvm.methods.count; ++i)
  {
    load_method_body( bvm.methods[i] );
  }

  // Now that all code has been loaded (and code buffer has finished sizing),
  // determine actual code addresses from offsets.
  for (int i=0; i<bvm.methods.count; ++i)
  {
    BardMethodInfo* m = bvm.methods[i];
    m->bytecode = bvm.code.data + m->bytecode_offset;
  }

  for (int i=0; i<bvm.address_offsets.count; ++i)
  {
    bvm.address_table.add( bvm.code.data + bvm.address_offsets[i] );
  }
}

void BardLoader::load_method_body( BardMethodInfo* m )
{
  this_method = m;

  int code_count = bvm.code.count;
  this_method->bytecode_offset = code_count;

  this_method->source_pos_offset = bvm.line_table.count;
  next_structure_id = -1;
  next_auto_id = -2;

  resolved_labels.clear();

  load_statement_list();

  if (unresolved_labels.count)
  {
    bard_throw_fatal_error("Unresolved labels in ",m->type_context->name,"::",m->signature,".");
  }

  if (this_method->native_handler)
  {
    // Embed the native handler in a minimal body - this makes it easy to handle
    // stack frames in a consistent way.  Since dynamic method calls can call
    // a VM method *or* a native method, we do need to support the stack frame
    // system.

    // Methods can be flagged as native at load time - undo any method body
    // that's loaded so far.
    bvm.code.count = code_count;

    write_op_i32( BARDOP_NATIVE_CALL, this_method->index );

    if (this_method->return_type)
    {
      if (this_method->return_type->is_reference()) 
      {
        write_op( BARDOP_RETURN_REF );
      }
      else
      {
        int size = this_method->return_type->stack_slots;
        if (size == 1) write_op( BARDOP_RETURN_8 );
        else           write_op_i32( BARDOP_RETURN_X, (BardInt32) size );
      }
    }
    else
    {
      write_op( BARDOP_RETURN_NIL );
    }
  }
  else
  {
    // Supply a catch-all return in case the user left off the return.

    if (this_method->return_type) 
    {
      write_op( BARDOP_MISSING_RETURN_ERROR );
    }
    else
    {
      write_op( BARDOP_RETURN_NIL );
    }
  }
  this_method->bytecode_limit = bvm.code.count;
}

BardTypeInfo* BardLoader::load_type()
{
  int index = reader.readX();
  if (index == -1) return NULL;

  if (index < 0 || index >= bvm.types.count)
  {
    bard_throw_fatal_error( "Type index out of bounds in loader." );
  }

  return bvm.types[index];
}

BardMethodInfo* BardLoader::load_method()
{
  int index = reader.readX();
  if (index == -1) return NULL;

  if (index < 0 || index >= bvm.methods.count)
  {
    bard_throw_fatal_error( "Method index out of bounds in loader." );
  }

  return bvm.methods[index];
}

char* BardLoader::load_id()
{
  int index = reader.readX();
  if (index < 0 || index >= bvm.identifiers.count)
  {
    bard_throw_fatal_error( "ID index out of bounds in loader." );
  }

  return bvm.identifiers[index];
}

BardLocalVarInfo* BardLoader::load_local()
{
  int index = reader.readX();
  if (index < this_method->parameters->count()) return this_method->parameters->get(index);
  else return this_method->local_vars[index - this_method->parameters->count()];
}

BardPropertyInfo* BardLoader::load_this_property()
{
  return this_method->type_context->properties[ reader.readX() ];
}


void BardLoader::must_consume_header( const char* header_id )
{
  int count = reader.read();
  for (int i=0; i<count; ++i)
  {
    if (reader.read() != header_id[i]) 
    {
      bard_throw_fatal_error( "Error reading header '", header_id, "'." );
    }
  }
  if (header_id[count] != 0)
  {
    bard_throw_fatal_error( "Error reading header '", header_id, "'." );
  }
}

void BardLoader::write_op( int op )
{
  if (op_history.count >= 4) op_history.remove(0);
  op_history.add( BardOpInfo(op,bvm.code.count) );

  bvm.code.add(op);
}

void BardLoader::write_i32( BardInt32 value )
{
  bvm.code.add( value );
}

void BardLoader::write_op_i32( int op, BardInt32 value )
{
  write_op(op);
  bvm.code.add( value );
}

void BardLoader::write_op_i64( int op, BardInt64 value )
{
  write_op(op);
  bvm.code.add( bvm.literal_table.count );
  bvm.literal_table.add( value );
}

int BardLoader::history( int num_back )
{
  if (op_history.count < num_back) return 0;
  return op_history[ op_history.count - num_back ].op;
}

void BardLoader::undo_op( int count )
{
  while (--count) op_history.remove_last();

  bvm.code.count = op_history.last().code_offset;
  op_history.remove_last();
}

void BardLoader::load_statement_list()
{
  int n = reader.readX();
  while (n--)
  {
    BardTypeInfo* type = load_statement();
    if (type)
    {
      // Get rid of bytes left on stack.
      if (type->is_reference()) write_op( BARDOP_POP_REF );
      else 
      {
        int size = type->stack_slots;
        if (size == 1) write_op( BARDOP_POP_8 );
        else           write_op_i32( BARDOP_POP_X, (BardInt32) size );
      }
    }
  }
}

BardTypeInfo* BardLoader::load_statement()
{
  for (;;)
  {
    int op = reader.readX();
    switch (op)
    {
      case BARDCMD_SET_FILENAME_AND_LINE:
        // filename index <= 0
        bvm.line_table.add( BardSourcePos( bvm.code.count, -reader.readX() ) );

        cur_line = reader.readX();
        bvm.line_table.add( BardSourcePos( bvm.code.count, cur_line ) );
        continue;

      case BARDCMD_SET_LINE:
        cur_line = reader.readX();
        bvm.line_table.add( BardSourcePos( bvm.code.count, cur_line ) );
        continue;

      case BARDCMD_INC_LINE:
        bvm.line_table.add( BardSourcePos( bvm.code.count, ++cur_line ) );
        continue;

      case BARDCMD_STRUCTURE_ID:
        next_structure_id = reader.readX();
        continue;

      case BARDCMD_RETURN_VALUE:
        {
          BardTypeInfo* type = load_expression();
          if (type->is_reference()) 
          {
            write_op( BARDOP_RETURN_REF );
          }
          else
          {
            int size = type->stack_slots;
            if (size == 1) write_op( BARDOP_RETURN_8 );
            else           write_op_i32( BARDOP_RETURN_X, (BardInt32) size );
          }
        }
        break;

      case BARDCMD_RETURN_NIL:
        write_op( BARDOP_RETURN_NIL );
        break;

      case BARDCMD_BLOCK:
        {
          int id = get_next_structure_id();
          load_statement_list();
          define_label( "end", id, 0 );
          close_label_id( id );
        }
        break;

      case BARDCMD_TRYCATCH:
        {
          int id = get_next_structure_id();
          int try_block_begin = bvm.code.count;

          load_statement_list(); 
          int try_block_end = bvm.code.count;
          
          int num_catches = reader.readX();
          if (num_catches) 
          {
            write_op( BARDOP_JUMP );
            write_label_address( "end", id, 0 );

            while (num_catches--)
            {
              int catch_var = reader.readX() - this_method->parameters->count();

              this_method->catches.add(
                  new BardCatchInfo(
                    this_method->local_vars[catch_var]->type,
                    try_block_begin, try_block_end,
                    bvm.code.count
                  )
                );

              // Insert code to copy the exception reference into the catch's
              // designated local variable.
              write_op_i32( 
                  BARDOP_WRITE_LOCAL_REF, 
                  (BardInt32) this_method->local_vars[catch_var]->offset 
                  );

              load_statement_list();

              write_op( BARDOP_JUMP );
              write_label_address( "end", id, 0 );
            }
          }

          define_label( "end", id, 0 );
          close_label_id( id );
        }
        break;

      case BARDCMD_THROW:
        load_ref_expression();
        write_op( BARDOP_THROW );
        break;

      case BARDCMD_IF:
        {
          int id = get_next_structure_id();
          int count = reader.readX();
          bool has_else = false;
          if (count < 0)
          {
            has_else = true;
            count = -count;
          }

          bool endif_relay;
          const char* end_label;
          if (count > 2)
          {
            end_label = "actual_end";
            endif_relay = true;
          }
          else
          {
            end_label = "end";
            endif_relay = false;
          }

          bool first = true;
          while (count)
          {
            // Define jump target for immediate backpatch but don't remember the 
            // label so as to work with any number of 'elseIf' statements.
            define_transient_label( "check", id, count );

            load_logical_expression();
            write_jump_if_false( "check", id, count-1 );
            load_statement_list();

            if (count > 1 || has_else)
            {
              write_op( BARDOP_JUMP );
              write_label_address( "end", id, 0 );
            }

            if (first)
            {
              first = false;
              if (endif_relay)
              {
                // Avoid accumulating a bunch of backpatch labels by putting a single
                // jump-to-end relay at the end of the first "if".
                define_label( "end", id, 0 );
                write_op( BARDOP_JUMP );
                write_label_address( "actual_end", id, 0 );
              }
            }

            count--;
          }

          // else
          define_label( "check", id, 0 );
          if (has_else) load_statement_list();

          define_label( end_label, id, 0 );
          close_label_id( id );
        }
        break;

      case BARDCMD_CONTINGENT:
        {
          int id = get_next_structure_id();
          load_statement_list();

          define_label( "satisfied", id, 0 );
          load_statement_list();
          write_op( BARDOP_JUMP );
          write_label_address( "end", id, 0 );

          define_label( "unsatisfied", id, 0 );
          load_statement_list();

          define_label( "end", id, 0 );
          close_label_id( id );
        }
        break;

      case BARDCMD_NECESSARY:
        {
          int id = reader.readX();
          load_logical_expression();
          write_jump_if_false( "unsatisfied", id, 0 );
        }
        break;

      case BARDCMD_SUFFICIENT:
        {
          int id = reader.readX();
          load_logical_expression();
          write_jump_if_true( "satisfied", id, 0 );
        }
        break;

      case BARDCMD_WHILE:
        {
          int id = get_next_structure_id();
          write_op( BARDOP_JUMP );
          write_label_address( "termination-test", id, 0 );

          define_label( "loop", id, 0 );
          load_statement_list();

          define_label( "termination-test", id, 0 );
          load_logical_expression();
          write_jump_if_true( "loop", id, 0 );

          define_label( "end", id, 0 );
          close_label_id( id );
        }
        break;

      case BARDCMD_LOOP:
        {
          int id = get_next_structure_id();

          define_label( "loop", id, 0 );
          define_label( "termination-test", id, 0 );
          load_statement_list();

          write_op( BARDOP_JUMP );
          write_label_address( "loop", id, 0 );

          define_label( "end", id, 0 );
          close_label_id( id );
        }
        break;

      case BARDCMD_ESCAPE:
        {
          int id = reader.readX();
          write_op( BARDOP_JUMP );
          write_label_address( "end", id, 0 );
        }
        break;

      case BARDCMD_NEXT_ITERATION:
        {
          int id = reader.readX();
          write_op( BARDOP_JUMP );
          write_label_address( "termination-test", id, 0 );
        }
        break;

      case BARDCMD_BREAKPOINT:
        {
          int id = reader.readX();
          write_op_i32( BARDOP_BREAKPOINT, (BardInt32) id );
        }
        break;

      case BARDCMD_LITERAL_STRING:
        {
          int index = reader.readX();
          write_op_i32( BARDOP_LITERAL_STRING, index );
          return bvm.type_string;
        }

      case BARDCMD_LITERAL_INT64:
        {
          BardInt64 value = reader.read64();
          switch (value)
          {
            case 1:  write_op( BARDOP_LITERAL_INTEGER_1 ); break;
            case 0:  write_op( BARDOP_LITERAL_INTEGER_0 ); break;
            case -1: write_op( BARDOP_LITERAL_INTEGER_NEG1 ); break;
            default:
              write_op_i64( BARDOP_LITERAL_8, value );
              break;
          }
          return bvm.type_int64;
        }

      case BARDCMD_LITERAL_INT32:
        {
          BardInt32 value = reader.readX();
          switch (value)
          {
            case 1:  write_op( BARDOP_LITERAL_INTEGER_1 ); break;
            case 0:  write_op( BARDOP_LITERAL_INTEGER_0 ); break;
            case -1: write_op( BARDOP_LITERAL_INTEGER_NEG1 ); break;
            default:
              write_op_i32( BARDOP_LITERAL_4, value );
              break;
          }
          return bvm.type_int32;
        }

      case BARDCMD_LITERAL_CHAR:
        {
          write_op_i32( BARDOP_LITERAL_4, (BardInt32) reader.readX() );
        }
        return bvm.type_char;

      case BARDCMD_LITERAL_BYTE:
        {
          write_op_i32( BARDOP_LITERAL_4, (BardInt32) reader.readX() );
        }
        return bvm.type_byte;

      case BARDCMD_LITERAL_REAL64:
        {
          BardInt64  value = reader.read64();
          BardReal64 real_value = *((BardReal64*) &value);
          if (real_value == 1.0)       write_op( BARDOP_LITERAL_REAL_1 );
          else if (real_value == 0.0)  write_op( BARDOP_LITERAL_REAL_0 );
          else if (real_value == -1.0) write_op( BARDOP_LITERAL_REAL_NEG1 );
          else
          {
            write_op_i64( BARDOP_LITERAL_8, value );
          }
        }
        return bvm.type_real64;

      case BARDCMD_LITERAL_REAL32:
        {
          write_op_i32( BARDOP_LITERAL_REAL32, (BardInt32) reader.readX() );
        }
        return bvm.type_real32;

      case BARDCMD_LITERAL_LOGICAL_TRUE:
        write_op( BARDOP_LITERAL_INTEGER_1 );
        return bvm.type_logical;

      case BARDCMD_LITERAL_LOGICAL_FALSE:
        write_op( BARDOP_LITERAL_INTEGER_0 );
        return bvm.type_logical;

      case BARDCMD_LITERAL_LOGICAL_VOID:
        write_op( BARDOP_LITERAL_INTEGER_NEG1 );
        return bvm.type_logical;

      case BARDCMD_LITERAL_NULL:
        write_op( BARDOP_LITERAL_NULL );
        return bvm.type_null;

      case BARDCMD_THIS_REFERENCE:
        write_op( BARDOP_READ_THIS_REF );
        return this_method->type_context;

      case BARDCMD_SINGLETON_REFERENCE:
        {
          BardTypeInfo* type = load_type();
          write_op_i32( BARDOP_READ_SINGLETON_REF, type->singleton_index );
          return type;
        }
        break;

      case BARDCMD_SINGLETON_WRITE:
        {
          BardTypeInfo* type = load_type();
          load_ref_expression();
          write_op_i32( BARDOP_WRITE_SINGLETON_REF, type->singleton_index );
        }
        break;

      case BARDCMD_PROPERTY_READ:
        {
          BardTypeInfo* context_type = load_ref_expression();
          int index = reader.readX();
          BardInt32 offset = context_type->properties[index]->offset;
          BardTypeInfo* var_type = context_type->properties[index]->type;

          int last_op = history(1);
          if (var_type->is_reference())
          {
            if (last_op == BARDOP_READ_THIS_REF)
            {
              undo_op();
              write_op_i32( BARDOP_READ_THIS_PROPERTY_REF, offset );
            }
            else
            {
              write_op_i32( BARDOP_READ_PROPERTY_REF, offset );
            }
          }
          else 
          {
            BardInt32 size = var_type->object_size;
            switch (size)
            {
              case 1:  
                write_op_i32(BARDOP_READ_PROPERTY_1U,offset);
                break;

              case 2:
                write_op_i32(BARDOP_READ_PROPERTY_2U,offset);
                break;
              case 4:  
                if (last_op == BARDOP_READ_THIS_REF)
                {
                  undo_op();
                  if (var_type == bvm.type_real32) write_op_i32( BARDOP_READ_THIS_PROPERTY_REAL32, offset );
                  else                             write_op_i32( BARDOP_READ_THIS_PROPERTY_4, offset );
                }
                else
                {
                  if (var_type == bvm.type_real32) write_op_i32( BARDOP_READ_PROPERTY_REAL32, offset );
                  else                             write_op_i32( BARDOP_READ_PROPERTY_4, offset );
                }
                break;
              case 8:  
                if (last_op == BARDOP_READ_THIS_REF)
                {
                  undo_op();
                  write_op_i32( BARDOP_READ_THIS_PROPERTY_8, offset );
                }
                else
                {
                  write_op_i32( BARDOP_READ_PROPERTY_8, offset );
                }
                break;
              default:
                if (last_op == BARDOP_READ_THIS_REF)
                {
                  undo_op();
                  write_op_i32( BARDOP_READ_THIS_PROPERTY_X, offset );
                  write_i32( size );
                }
                else
                {
                  write_op_i32( BARDOP_READ_PROPERTY_X, offset );
                  write_i32( size );
                }
                break;
            }
          }
          return var_type;
        }
        break;

      case BARDCMD_COMPOUND_PROPERTY_READ:
        {
          BardTypeInfo* context_type = load_compound_expression();
          int index = reader.readX();
          BardInt32 offset = context_type->properties[index]->offset;
          BardTypeInfo* var_type = context_type->properties[index]->type;
          BardInt32 slots = var_type->stack_slots;

          // Don't need to do anything if the compound element is the same size
          // as the compound itself.
          if (slots == context_type->stack_slots) return var_type;

          int last_op = history(0);
          switch (slots)
          {
            case 1:  
              if (last_op == BARDOP_READ_THIS_PROPERTY_X)
              {
                offset += bvm.code[bvm.code.count-2];
                undo_op();
                write_op_i32( BARDOP_READ_THIS_PROPERTY_8, offset );
                return var_type;
              }
              else if (last_op == BARDOP_READ_PROPERTY_X)
              {
                offset += bvm.code[bvm.code.count-2];
                undo_op();
                write_op_i32( BARDOP_READ_PROPERTY_8, offset );
                return var_type;
              }
              else if (last_op == BARDOP_READ_LOCAL_X)
              {
                offset += bvm.code[bvm.code.count-2];
                undo_op();
                write_op_i32( BARDOP_READ_LOCAL_8, offset );
                return var_type;
              }
              else
              {
                write_op( BARDOP_READ_COMPOUND_8 ); 
              }
              break;

            default:
              if (last_op == BARDOP_READ_THIS_PROPERTY_X || last_op == BARDOP_READ_PROPERTY_X
                  || last_op == BARDOP_READ_LOCAL_X)
              {
                bvm.code[bvm.code.count-2] += offset;
                bvm.code[bvm.code.count-1] = slots;
                return var_type;
              }
              else
              {
                write_op( BARDOP_READ_COMPOUND_X );
                write_i32( (context_type->object_size)>>3 );
                write_i32( offset );
                write_i32( slots );
                return var_type;
              }
              break;
          }
          write_i32( context_type->stack_slots );
          write_i32( offset );
          return var_type;
        }
        break;

      case BARDCMD_LOCAL_VAR_READ:
        {
          BardInt32 offset;
          BardTypeInfo* var_type;
          int index = reader.readX();
          if (index < this_method->parameters->count())
          {
            var_type = this_method->parameters->get(index)->type;
            offset = this_method->parameters->get(index)->offset;
          }
          else
          {
            index -= this_method->parameters->count();
            var_type = this_method->local_vars[index]->type;
            offset = this_method->local_vars[index]->offset;
          }

          if (var_type->is_reference())
          {
            write_op_i32( BARDOP_READ_LOCAL_REF, offset );
          }
          else 
          {
            BardInt32 size = var_type->stack_slots;
            switch (size)
            {
              case 1:  write_op_i32( BARDOP_READ_LOCAL_8, offset ); break;
              default:
                write_op_i32( BARDOP_READ_LOCAL_X, offset );
                write_i32( size );
                break;
            }
          }
          return var_type;
        }
        break;

      case BARDCMD_PROPERTY_WRITE:
        {
          BardTypeInfo* context_type = load_ref_expression();

          int index = reader.readX();
          bool read_this = false;
          BardTypeInfo* var_type = context_type->properties[index]->type;
          BardInt32 offset = context_type->properties[index]->offset;
          BardInt32 size = var_type->object_size;

          if ( (history(0) == BARDOP_READ_THIS_REF)
              && (var_type->is_reference() || size >= 4) )
          {
            read_this = true;
            undo_op();
          }

          load_expression();

          if (read_this)
          {
            int write_op;
            if (var_type->is_reference())         write_op = BARDOP_WRITE_THIS_PROPERTY_REF;
            else if (var_type == bvm.type_real32) write_op = BARDOP_WRITE_THIS_PROPERTY_REAL32;
            else if (size == 4)                   write_op = BARDOP_WRITE_THIS_PROPERTY_4;
            else if (size == 8)                   write_op = BARDOP_WRITE_THIS_PROPERTY_8;
            else                                  write_op = BARDOP_WRITE_THIS_PROPERTY_X;
            write_op_i32( write_op, offset );
            if (write_op == BARDOP_WRITE_THIS_PROPERTY_X) 
            {
              write_i32( size );
            }
          }
          else
          {
            if (var_type->is_reference())
            {
              write_op_i32( BARDOP_WRITE_PROPERTY_REF, offset );
            }
            else if (var_type == bvm.type_real32)
            {
              write_op_i32( BARDOP_WRITE_PROPERTY_REAL32, offset ); break;
            }
            else
            {
              switch (size)
              {
                case 1: write_op_i32( BARDOP_WRITE_PROPERTY_1, offset ); break;
                case 2: write_op_i32( BARDOP_WRITE_PROPERTY_2, offset ); break;
                case 4: write_op_i32( BARDOP_WRITE_PROPERTY_4, offset ); break;
                case 8: write_op_i32( BARDOP_WRITE_PROPERTY_8, offset ); break;
                default:
                  write_op_i32( BARDOP_WRITE_PROPERTY_X, offset );
                  write_i32( size );
                  break;
              }
            }
          }
        }
        break;

      case BARDCMD_LOCAL_VAR_WRITE:
        {
          BardInt32 offset;
          BardTypeInfo* var_type;
          int index = reader.readX();
          if (index < this_method->parameters->count())
          {
            var_type = this_method->parameters->get(index)->type;
            offset = this_method->parameters->get(index)->offset;
          }
          else
          {
            index -= this_method->parameters->count();
            var_type = this_method->local_vars[index]->type;
            offset = this_method->local_vars[index]->offset;
          }

          load_expression();

          if (var_type->is_reference())
          {
            write_op_i32( BARDOP_WRITE_LOCAL_REF, offset );
          }
          else
          {
            BardInt32 slots = var_type->stack_slots;
            switch (slots)
            {
              case 1:  write_op_i32( BARDOP_WRITE_LOCAL_8, offset ); break;
              default:
                write_op_i32( BARDOP_WRITE_LOCAL_X, offset );
                write_i32( slots );
                break;
            }
          }
        }
        break;

      case BARDCMD_STATIC_CALL:
        {
          BardMethodInfo* m  = load_method();
          load_ref_expression();

          BardInt32 obj_offset = m->parameters->num_ref_params - 1;

          int count = m->parameters->count();
          while (count--)
          {
            load_expression();
          }

          if ( !(m->qualifiers & BARD_QUALIFIER_EMPTY_BODY) || m->return_type || m->native_handler)
          {
            write_op_i32( BARDOP_STATIC_CALL, m->index );
            write_i32( obj_offset );
          }
          else
          {
            write_op_i32( BARDOP_FAUX_STATIC_CALL, m->index );
          }
          return m->return_type;
        }
        break;

      case BARDCMD_DYNAMIC_CALL:
        {
          BardMethodInfo* m  = load_method();
          BardInt32 dispatch_table_offset = reader.readX();
          load_ref_expression();

          int count = m->parameters->count();
          while (count--)
          {
            load_expression();
          }

          BardInt32 obj_offset = m->parameters->num_ref_params - 1;
          write_op_i32( BARDOP_DYNAMIC_CALL, obj_offset );
          write_i32( dispatch_table_offset );

          return m->return_type;
        }
        break;

      case BARDCMD_NEW_OBJECT:
        {
          BardMethodInfo* m  = load_method();
          BardTypeInfo* type = load_type();

          if ( !(type->method_init_object->qualifiers & BARD_QUALIFIER_EMPTY_BODY)
              || type->method_init_object->native_handler)
          {
            write_op_i32( BARDOP_NEW_OBJECT, type->index );
          }
          else
          {
            write_op_i32( BARDOP_NEW_OBJECT_NO_INIT, type->index );
          }

          int count = m->parameters->count();
          while (count--)
          {
            load_expression();
          }

          if (!(m->qualifiers & BARD_QUALIFIER_EMPTY_BODY) || m->return_type || m->native_handler)
          {
            write_op_i32( BARDOP_STATIC_CALL, m->index );
            write_i32( m->parameters->num_ref_params - 1 );
          }
          else
          {
            if (m->parameters->count() == 0)
            {
              write_op( BARDOP_POP_REF );
            }
            else
            {
              write_op_i32( BARDOP_FAUX_STATIC_CALL, m->index );
            }
          }

          return type;
        }

      case BARDCMD_NEW_COMPOUND:
        {
          BardTypeInfo* type = load_type();

          int count = type->properties.count;
          while (count--)
          {
            load_expression();
          }

          // Just evaluating all the new compound parameters in reverse order results in a new
          // compound on the stack - no further action is required.
          return type;
        }

      case BARDCMD_ARRAY_CREATE:
        {
          BardTypeInfo* type = load_type();
          load_int32_expression();
          write_op_i32( BARDOP_NEW_ARRAY, type->index );
          return type;
        }
        break;

      case BARDCMD_ARRAY_GET:
        {
          BardTypeInfo* context_type = load_ref_expression();
          load_int32_expression();

          if (context_type->element_type->is_reference())
          {
            write_op( BARDOP_ARRAY_READ_REF );
          }
          else if (context_type->element_type == bvm.type_real32)
          {
            write_op( BARDOP_ARRAY_READ_REAL32 );
          }
          else
          {
            int size = context_type->element_size;

            switch (size)
            {
              case 1:
                write_op(BARDOP_ARRAY_READ_1U);
                break;
              case 2:
                write_op(BARDOP_ARRAY_READ_2U);
                break;
              case 4:  write_op( BARDOP_ARRAY_READ_4 ); break;
              case 8:  write_op( BARDOP_ARRAY_READ_8 ); break;
              default: 
                // Arrays know their own data size so we'll figure it out from that.
                write_op( BARDOP_ARRAY_READ_X ); break;
            }
          }
          return (BardTypeInfo*) context_type->element_type;      
        }
		

      case BARDCMD_ARRAY_SET:
        {
          BardTypeInfo* context_type = load_ref_expression();
          load_int32_expression(); // index expr
          load_expression(); // value expr

          if (context_type->element_type->is_reference())
          {
            write_op( BARDOP_ARRAY_WRITE_REF );
          }
          else if (context_type->element_type == bvm.type_real32)
          {
            write_op( BARDOP_ARRAY_WRITE_REAL32 );
          }
          else
          {
            switch (context_type->element_size)
            {
              case 1:  write_op( BARDOP_ARRAY_WRITE_1 ); break;
              case 2:  write_op( BARDOP_ARRAY_WRITE_2 ); break;
              case 4:  write_op( BARDOP_ARRAY_WRITE_4 ); break;
              case 8:  write_op( BARDOP_ARRAY_WRITE_8 ); break;
              default: write_op( BARDOP_ARRAY_WRITE_X ); break;
            }
          }
        }
        break;

      case BARDCMD_NARROWING_CAST:
        {
          BardTypeInfo* expr_type = load_ref_expression();
          BardTypeInfo* to_type = load_type();
          if (expr_type != bvm.type_null)
          {
            write_op_i32( BARDOP_TYPECHECK, to_type->index );
          }
          return to_type;
        }
        break;

      case BARDCMD_WIDENING_CAST:
        {
          load_ref_expression();
          return load_type();
        }
        break;

      case BARDCMD_CAST_TO_INT64:
        {
          BardTypeInfo* type = load_expression();
          if (type->is_real()) write_op( BARDOP_CAST_REAL_TO_INTEGER );
        }
        return bvm.type_int64;

      case BARDCMD_CAST_TO_INT32:
        {
          BardTypeInfo* type = load_expression();
          if (type->is_real()) write_op( BARDOP_CAST_REAL_TO_INTEGER );
          else if (type == bvm.type_int64) write_op( BARDOP_CAST_INTEGER_TO_I32 );
        }
        return bvm.type_int32;

      case BARDCMD_CAST_TO_CHAR:
        {
          load_expression();
          write_op( BARDOP_CAST_INTEGER_TO_CHAR );
        }
        return bvm.type_char;

      case BARDCMD_CAST_TO_BYTE:
        {
          load_expression();
          write_op( BARDOP_CAST_INTEGER_TO_BYTE );
        }
        return bvm.type_byte;

      case BARDCMD_CAST_TO_REAL64:
        {
          BardTypeInfo* type = load_expression();
          if (type->is_int32_64()) write_op( BARDOP_CAST_INTEGER_TO_REAL );
        }
        return bvm.type_real64;

      case BARDCMD_CAST_TO_REAL32:
        {
          BardTypeInfo* type = load_expression();
          if (type->is_int32_64()) write_op( BARDOP_CAST_INTEGER_TO_REAL );
        }
        return bvm.type_real32;

      case BARDCMD_CAST_TO_LOGICAL:
        {
          BardTypeInfo* type = load_expression();
          if (type == bvm.type_int32)        write_op( BARDOP_CAST_INTEGER_TO_LOGICAL );
          else if (type == bvm.type_real64)  write_op( BARDOP_CAST_REAL_TO_LOGICAL );
        }
        return bvm.type_logical;

      case BARDCMD_AS:
        {
          load_expression();
          BardTypeInfo* of_type = load_type();
          if (of_type->is_reference())
          {
            write_op_i32( BARDOP_AS_REF, of_type->index );
          }
          // No operation is required for non-references.
          return of_type;
        }
        break;

      case BARDCMD_COERCE_AS:
        {
          load_expression();
          BardTypeInfo* of_type = load_type();
          return of_type;
        }
        break;

      case BARDCMD_CMP_INSTANCE_OF:
        {
          load_ref_expression();
          write_op_i32( BARDOP_CMP_INSTANCE_OF, load_type()->index );
          return bvm.type_logical;
        }
        break;

      case BARDCMD_CMP_IS:
        {
          load_ref_expression();
          load_ref_expression();
          write_op( BARDOP_CMP_EQ_REF );
        }
        return bvm.type_logical;

      case BARDCMD_CMP_IS_NOT:
        {
          load_ref_expression();
          load_ref_expression();
          write_op( BARDOP_CMP_NE_REF );
        }
        return bvm.type_logical;

      case BARDCMD_CMP_IS_NULL:
        {
          load_ref_expression();
          write_op( BARDOP_CMP_EQ_REF_NULL );
        }
        return bvm.type_logical;

      case BARDCMD_CMP_IS_NOT_NULL:
        {
          load_ref_expression();
          write_op( BARDOP_CMP_NE_REF_NULL );
        }
        return bvm.type_logical;

      case BARDCMD_CMP_EQ:
        {
          BardTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( BARDOP_CMP_EQ_REAL );
          else if (type->is_integer()) write_op( BARDOP_CMP_EQ_INTEGER );
          else write_op_i32( BARDOP_CMP_EQ_X, (BardInt32) (type->stack_slots) );
        }
        return bvm.type_logical;

      case BARDCMD_CMP_NE:
        {
          BardTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( BARDOP_CMP_NE_REAL );
          else if (type->is_integer()) write_op( BARDOP_CMP_NE_INTEGER );
          else write_op_i32( BARDOP_CMP_NE_X, (BardInt32) (type->stack_slots) );
        }
        return bvm.type_logical;

      case BARDCMD_CMP_LE:
        {
          BardTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( BARDOP_CMP_LE_REAL );
          else write_op( BARDOP_CMP_LE_INTEGER );
        }
        return bvm.type_logical;

      case BARDCMD_CMP_LT:
        {
          BardTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( BARDOP_CMP_LT_REAL );
          else write_op( BARDOP_CMP_LT_INTEGER );
        }
        return bvm.type_logical;

      case BARDCMD_CMP_GT:
        {
          BardTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( BARDOP_CMP_GT_REAL );
          else write_op( BARDOP_CMP_GT_INTEGER );
        }
        return bvm.type_logical;

      case BARDCMD_CMP_GE:
        {
          BardTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( BARDOP_CMP_GE_REAL );
          else write_op( BARDOP_CMP_GE_INTEGER );
        }
        return bvm.type_logical;

      case BARDCMD_NOT:
        {
          BardTypeInfo* type = load_expression();

          if (type == bvm.type_logical) write_op( BARDOP_NOT_LOGICAL );
          else                              write_op( BARDOP_NOT_INTEGER );
          return type;
        }

      case BARDCMD_NEGATE:
        {
          BardTypeInfo* type = load_expression();

          if (type->is_real()) write_op( BARDOP_NEGATE_REAL );
          else                 write_op( BARDOP_NEGATE_INTEGER );
          return type;
        }

      case BARDCMD_ADD:
        {
          BardTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( BARDOP_ADD_REAL );
          else                 write_op( BARDOP_ADD_INTEGER );
          return type;
        }

      case BARDCMD_SUB:
        {
          BardTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( BARDOP_SUB_REAL );
          else                 write_op( BARDOP_SUB_INTEGER );
          return type;
        }

      case BARDCMD_MUL:
        {
          BardTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( BARDOP_MUL_REAL );
          else                 write_op( BARDOP_MUL_INTEGER );
          return type;
        }

      case BARDCMD_DIV:
        {
          BardTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( BARDOP_DIV_REAL );
          else                 write_op( BARDOP_DIV_INTEGER );
          return type;
        }

      case BARDCMD_MOD:
        {
          BardTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( BARDOP_MOD_REAL );
          else                 write_op( BARDOP_MOD_INTEGER );
          return type;
        }

      case BARDCMD_EXP:
        {
          BardTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( BARDOP_EXP_REAL );
          else                 write_op( BARDOP_EXP_INTEGER );
          return type;
        }

      case BARDCMD_AND:
        {
          BardTypeInfo* type = load_expression();
          load_expression();

          if (type->is_logical())   write_op( BARDOP_AND_LOGICAL );
          else                      write_op( BARDOP_AND_INTEGER );
          return type;
        }

      case BARDCMD_OR:
        {
          BardTypeInfo* type = load_expression();
          load_expression();

          if (type->is_logical())   write_op( BARDOP_OR_LOGICAL );
          else                      write_op( BARDOP_OR_INTEGER );
          return type;
        }

      case BARDCMD_XOR:
        {
          BardTypeInfo* type = load_expression();
          load_expression();

          if (type->is_logical())          write_op( BARDOP_XOR_LOGICAL );
          else if (type == bvm.type_int64) write_op( BARDOP_XOR_INT64 );
          else                             write_op( BARDOP_XOR_INT32 );

          return type;
        }

      case BARDCMD_LAZY_AND:
        {
          int id = get_next_structure_id();
          load_logical_expression();
          write_jump_if_true( "evaluate rhs", id, 0 );
          write_op( BARDOP_LITERAL_INTEGER_0 );
          write_op( BARDOP_JUMP );
          write_label_address( "end", id, 0 );

          define_label( "evaluate rhs", id, 0 );
          load_logical_expression();
          define_label( "end", id, 0 );
          close_label_id( id );

          return bvm.type_logical;
        }

      case BARDCMD_LAZY_OR:
        {
          int id = get_next_structure_id();
          load_logical_expression();
          write_jump_if_false( "evaluate rhs", id, 0 );
          write_op( BARDOP_LITERAL_INTEGER_1 );
          write_op( BARDOP_JUMP );
          write_label_address( "end", id, 0 );

          define_label( "evaluate rhs", id, 0 );
          load_logical_expression();
          define_label( "end", id, 0 );
          close_label_id( id );

          return bvm.type_logical;
        }

      case BARDCMD_LEFT_SHIFTED:
        {
          BardTypeInfo* type = load_expression();
          load_int32_expression();

          if (type == bvm.type_int64) write_op( BARDOP_SHL_INT64 );
          else                        write_op( BARDOP_SHL_INT32 );

          return type;
        }


      case BARDCMD_RIGHT_SHIFTED:
        {
          BardTypeInfo* type = load_expression();
          load_int32_expression();

          if (type == bvm.type_int64) write_op( BARDOP_SHR_INT64 );
          else                        write_op( BARDOP_SHR_INT32 );

          return type;
        }


      case BARDCMD_RIGHT_XSHIFTED:
        {
          BardTypeInfo* type = load_expression();
          load_int32_expression();

          if (type == bvm.type_int64) write_op( BARDOP_SHRX_INT64 );
          else                        write_op( BARDOP_SHRX_INT32 );

          return type;
        }

      case BARDCMD_THIS_ADD_AND_ASSIGN:
        {
          BardPropertyInfo* p = load_this_property();
          BardTypeInfo* type = p->type;

          load_expression();

          if (type->is_real()) write_op_i32( BARDOP_THIS_ADD_ASSIGN_R64, (BardInt32) p->offset );
          else                 write_op_i32( BARDOP_THIS_ADD_ASSIGN_I32, (BardInt32) p->offset );
        }
        break;

      case BARDCMD_THIS_SUB_AND_ASSIGN:
        {
          BardPropertyInfo* p = load_this_property();
          BardTypeInfo* type = p->type;

          load_expression();

          if (type->is_real()) write_op_i32( BARDOP_THIS_SUB_ASSIGN_R64, (BardInt32) p->offset );
          else                 write_op_i32( BARDOP_THIS_SUB_ASSIGN_I32, (BardInt32) p->offset );
        }
        break;

      case BARDCMD_THIS_MUL_AND_ASSIGN:
        {
          BardPropertyInfo* p = load_this_property();
          BardTypeInfo* type = p->type;

          load_expression();

          if (type->is_real()) write_op_i32( BARDOP_THIS_MUL_ASSIGN_R64, (BardInt32) p->offset );
          else                 write_op_i32( BARDOP_THIS_MUL_ASSIGN_I32, (BardInt32) p->offset );
        }
        break;

      case BARDCMD_THIS_DIV_AND_ASSIGN:
        {
          BardPropertyInfo* p = load_this_property();
          BardTypeInfo* type = p->type;

          write_op( BARDOP_DUPLICATE_REF );

          if (type->object_size == 8)       write_op_i32( BARDOP_READ_THIS_PROPERTY_8, (BardInt32) p->offset );
          else if (type == bvm.type_real32) write_op_i32( BARDOP_READ_THIS_PROPERTY_REAL32, (BardInt32) p->offset );
          else                              write_op_i32( BARDOP_READ_THIS_PROPERTY_4, (BardInt32) p->offset );

          load_expression();

          if (type->is_real()) write_op( BARDOP_DIV_REAL );
          else                 write_op( BARDOP_DIV_INTEGER );

          if (type->object_size == 8)       write_op_i32( BARDOP_WRITE_THIS_PROPERTY_8, (BardInt32) p->offset );
          else if (type == bvm.type_real32) write_op_i32( BARDOP_WRITE_THIS_PROPERTY_REAL32, (BardInt32) p->offset );
          else                              write_op_i32( BARDOP_WRITE_THIS_PROPERTY_4, (BardInt32) p->offset );
        }
        break;

      case BARDCMD_THIS_MOD_AND_ASSIGN:
        {
          BardPropertyInfo* p = load_this_property();
          BardTypeInfo* type = p->type;

          write_op( BARDOP_DUPLICATE_REF );

          if (type->object_size == 8)       write_op_i32( BARDOP_READ_THIS_PROPERTY_8, (BardInt32) p->offset );
          else if (type == bvm.type_real32) write_op_i32( BARDOP_READ_THIS_PROPERTY_REAL32, (BardInt32) p->offset );
          else                              write_op_i32( BARDOP_READ_THIS_PROPERTY_4, (BardInt32) p->offset );

          load_expression();

          if (type->is_real()) write_op( BARDOP_MOD_REAL );
          else                 write_op( BARDOP_MOD_INTEGER );

          if (type->object_size == 8)       write_op_i32( BARDOP_WRITE_THIS_PROPERTY_8, (BardInt32) p->offset );
          else if (type == bvm.type_real32) write_op_i32( BARDOP_WRITE_THIS_PROPERTY_REAL32, (BardInt32) p->offset );
          else                              write_op_i32( BARDOP_WRITE_THIS_PROPERTY_4, (BardInt32) p->offset );
        }
        break;


      case BARDCMD_LOCAL_VAR_ADD_AND_ASSIGN:
        {
          BardLocalVarInfo* v = load_local();
          BardTypeInfo* type = v->type;
          BardInt32 offset = v->offset;

          load_expression();

          if (type->is_real()) write_op_i32( BARDOP_LOCAL_ADD_ASSIGN_REAL, offset );
          else                 write_op_i32( BARDOP_LOCAL_ADD_ASSIGN_INTEGER, offset );
        }
        break;

      case BARDCMD_LOCAL_VAR_SUB_AND_ASSIGN:
        {
          BardLocalVarInfo* v = load_local();
          BardTypeInfo* type = v->type;
          BardInt32 offset = v->offset;

          load_expression();

          if (type->is_real()) write_op_i32( BARDOP_LOCAL_SUB_ASSIGN_REAL, offset );
          else                 write_op_i32( BARDOP_LOCAL_SUB_ASSIGN_INTEGER, offset );
        }
        break;

      case BARDCMD_LOCAL_VAR_MUL_AND_ASSIGN:
        {
          BardLocalVarInfo* v = load_local();
          BardTypeInfo* type = v->type;
          BardInt32 offset = v->offset;

          load_expression();

          if (type->is_real()) write_op_i32( BARDOP_LOCAL_MUL_ASSIGN_REAL, offset );
          else                 write_op_i32( BARDOP_LOCAL_MUL_ASSIGN_INTEGER, offset );
        }
        break;

      case BARDCMD_LOCAL_VAR_DIV_AND_ASSIGN:
        {
          BardLocalVarInfo* v = load_local();
          BardTypeInfo* type = v->type;
          BardInt32 offset = v->offset;

          write_op_i32( BARDOP_READ_LOCAL_8, offset );

          load_expression();

          if (type->is_real()) write_op( BARDOP_DIV_REAL );
          else                 write_op( BARDOP_DIV_INTEGER );

          write_op_i32( BARDOP_WRITE_LOCAL_8, offset );
        }
        break;

      case BARDCMD_LOCAL_VAR_MOD_AND_ASSIGN:
        {
          BardLocalVarInfo* v = load_local();
          BardTypeInfo* type = v->type;
          BardInt32 offset = v->offset;

          write_op_i32( BARDOP_READ_LOCAL_8, offset );

          load_expression();

          if (type->is_real()) write_op( BARDOP_MOD_REAL );
          else                 write_op( BARDOP_MOD_INTEGER );

          write_op_i32( BARDOP_WRITE_LOCAL_8, offset );
        }
        break;

      case BARDCMD_THIS_INCREMENT:
        {
          BardPropertyInfo* p = load_this_property();
          BardInt32 offset = p->offset;

          if (p->type->is_real()) write_op_i32( BARDOP_THIS_INCREMENT_R64, offset );
          else                    write_op_i32( BARDOP_THIS_INCREMENT_I32, offset );
        }
        break;

      case BARDCMD_THIS_DECREMENT:
        {
          BardPropertyInfo* p = load_this_property();
          BardInt32 offset = p->offset;

          if (p->type->is_real()) write_op_i32( BARDOP_THIS_DECREMENT_R64, offset );
          else                    write_op_i32( BARDOP_THIS_DECREMENT_I32, offset );
        }
        break;

      case BARDCMD_LOCAL_INCREMENT:
        {
          BardLocalVarInfo* v = load_local();
          BardInt32 offset = v->offset;

          if (v->type->is_real()) write_op_i32( BARDOP_LOCAL_INCREMENT_REAL, offset );
          else                    write_op_i32( BARDOP_LOCAL_INCREMENT_INTEGER, offset );
        }
        break;

      case BARDCMD_LOCAL_DECREMENT:
        {
          BardLocalVarInfo* v = load_local();
          BardInt32 offset = v->offset;

          if (v->type->is_real()) write_op_i32( BARDOP_LOCAL_DECREMENT_REAL, offset );
          else                    write_op_i32( BARDOP_LOCAL_DECREMENT_INTEGER, offset );
        }
        break;

      case BARDCMD_ARRAY_DUPLICATE:
        {
          BardTypeInfo* array_type = load_expression();
          write_op( BARDOP_ARRAY_DUPLICATE );
          return array_type;
        }
        break;

      default:
        {
          char buffer[80];
          sprintf( buffer, "Unhandled opcode while loading: %d.", op );
          bard_throw_fatal_error( buffer );
        }
    }
    break;

  }

  return 0;
}

BardTypeInfo* BardLoader::load_expression()
{
  BardTypeInfo* result = load_statement();
  bard_assert( result != NULL, "Expression expected." );
  return result;
}

BardTypeInfo* BardLoader::load_ref_expression()
{
  BardTypeInfo* result = load_statement();
  bard_assert( result && result->is_reference(), "Reference expression expected." );
  return result;
}

void BardLoader::load_logical_expression()
{
  BardTypeInfo* result = load_statement();
  bard_assert( result && result->is_logical(), "Logical expression expected." );
}

void BardLoader::load_int32_expression()
{
  BardTypeInfo* result = load_statement();
  bard_assert( result && result == bvm.type_int32, "Int32 expression expected." );
}

BardTypeInfo* BardLoader::load_compound_expression()
{
  BardTypeInfo* result = load_statement();
  bard_assert( result && result->is_compound(), "Int32 expression expected." );
  return result;
}

int BardLoader::get_next_structure_id()
{
  if (next_structure_id == -1)
  {
    return next_auto_id--;
  }
  else
  {
    int result = next_structure_id;
    next_structure_id = -1;
    return result;
  }
}

void BardLoader::define_label( const char* name, int id, int n )
{
  BardCodeLabel label(name,id,n);

  int address_index;
  if ( define_transient_label(name,id,n,&address_index) )
  {
    label.address_index = address_index;
  }
  else
  {
    label.address_index = bvm.address_offsets.count;
    bvm.address_offsets.add( bvm.code.count );
  }

  resolved_labels.add(label);
}

bool BardLoader::define_transient_label( const char* name, int id, int n, int* index_ptr )
{
  BardCodeLabel label(name,id,n);

  // Backpatch any unresolved references to this label.  They will all
  // point to the same address table entry, so fixing one will fix
  // all - as such, only one backpatch request is actually saved.
  for (int i=0; i<unresolved_labels.count; ++i)
  {
    if (unresolved_labels[i].equals(label))
    {
      label.address_index = unresolved_labels[i].address_index;
      if (index_ptr) *index_ptr = label.address_index;
      bvm.address_offsets[label.address_index] = bvm.code.count;
      unresolved_labels.remove(i);
      return true;
    }
  }

  return false;
}

void BardLoader::write_label_address( const char* name, int id, int n )
{
  BardCodeLabel label(name,id,n);

  // See if the label is already defined.
  for (int i=0; i<resolved_labels.count; ++i)
  {
    if (resolved_labels[i].equals(label))
    {
      write_i32( resolved_labels[i].address_index );
      return;
    }
  }

  // Share an existing backpatch address index if possible.
  for (int i=0; i<unresolved_labels.count; ++i)
  {
    if (unresolved_labels[i].equals(label))
    {
      write_i32( unresolved_labels[i].address_index );
      return;
    }
  }

  // Undefined - write_op a placeholder & backpatch later.
  label.address_index = bvm.address_offsets.count;
  bvm.address_offsets.add(0);
  unresolved_labels.add( label );

  write_op(label.address_index);
}

void BardLoader::close_label_id( int id )
{
  // Ensure every label in the backpatch list has been taken care of.
  for (int i=0; i<unresolved_labels.count; ++i)
  {
    bool result = (unresolved_labels[i].id == id);
    bard_assert( result==false, "Pending label closed without backpatch." );
  }

  while (resolved_labels.count && resolved_labels.last().id == id)
  {
    resolved_labels.remove_last();
  }
}

void BardLoader::write_jump_if_true( const char* name, int id, int n )
{
  int op = BARDOP_JUMP_IF_TRUE;
  if (history(0) == BARDOP_CMP_EQ_REF_NULL)
  {
    undo_op();
    op = BARDOP_JUMP_IF_NULL_REF;
  }
  else if (history(0) == BARDOP_CMP_NE_REF_NULL)
  {
    undo_op();
    op = BARDOP_JUMP_IF_REF;
  }

  write_op( op );
  write_label_address( name, id, n );
}

void BardLoader::write_jump_if_false( const char* name, int id, int n )
{
  int op = BARDOP_JUMP_IF_FALSE;
  if (history(0) == BARDOP_CMP_EQ_REF_NULL)
  {
    undo_op();
    op = BARDOP_JUMP_IF_REF;
  }
  else if (history(0) == BARDOP_CMP_NE_REF_NULL)
  {
    undo_op();
    op = BARDOP_JUMP_IF_NULL_REF;
  }
  write_op( op );
  write_label_address( name, id, n );
}

void bard_set_raw_exe_filepath( char* filepath )
{
  bvm.raw_exe_filepath = filepath;
}

void bard_set_command_line_args( char** argv, int argc )
{
  bvm.set_command_line_args( argv, argc );
}

void bard_throw_file_error()
{
  bvm.throw_exception( bvm.type_file_error );
}

void bard_throw_file_error( char* filename )
{
  bvm.throw_exception( bvm.type_file_error, filename );
}

void bard_throw_file_not_found_error( char* filename )
{
  bvm.throw_exception( bvm.type_file_not_found_error, filename );
}

void bard_throw_no_next_value_error()
{
  bvm.throw_exception( bvm.type_no_next_value_error );
}

void bard_throw_socket_error()
{
  bvm.throw_exception( bvm.type_socket_error );
}

void bard_throw_invalid_operand_error()
{
  bvm.throw_exception( bvm.type_invalid_operand_error );
}

