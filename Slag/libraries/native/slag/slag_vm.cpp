//=============================================================================
//  slag_vm.cpp
//
//  2010.11.26 by Abe Pralle
//
//  $(SLAG_VERSION)
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

#include "slag.h"
#include "slag_mm.h"
#include "slag_vm.h"
#include "slag_stdlib.h"

SlagVM vm;

//=============================================================================
//  SlagGenericObject
//=============================================================================
int SlagGenericObject::property_offset( const char* name )
{
  return type->must_find_property(name)->offset;
}

void* SlagGenericObject::property_address( const char* name )
{
  return (void*)(((char*)this) + type->must_find_property(name)->offset);
}

void* SlagGenericObject::property_address( int offset )
{
  return (void*)(((char*)this) + offset);
}

void SlagGenericObject::set_ref( const char* id, SlagObject* object )
{
  SlagObject** property = (SlagObject**) (((char*)this) + type->must_find_property(id)->offset);
  if (*property) --((*property)->reference_count);
  *property = object;
  if (object) ++object->reference_count;
}

void SlagGenericObject::set_ref( int offset, SlagObject* object )
{
  SlagObject** property = (SlagObject**) (((char*)this) + offset);
  if (*property) --((*property)->reference_count);
  *property = object;
  if (object) ++object->reference_count;
}

SlagObject* SlagGenericObject::get_ref( const char* name )
{
  SlagObject** property = (SlagObject**) (((char*)this) + type->must_find_property(name)->offset);
  return *property;
}

SlagObject* SlagGenericObject::get_ref( int offset )
{
  SlagObject** property = (SlagObject**) (((char*)this) + offset);
  return *property;
}


//=============================================================================
// SlagTypeInfo
//=============================================================================
bool SlagTypeInfo::is_real()
{
  return (this == vm.type_real64 || this == vm.type_real32);
}

bool SlagTypeInfo::is_integer()
{
  return (this == vm.type_int64 || this == vm.type_int32
    || this == vm.type_char || this == vm.type_byte);
}

bool SlagTypeInfo::is_int32_64()
{
  return (this == vm.type_int64 || this == vm.type_int32);
}

bool SlagTypeInfo::is_logical()
{
  return (this == vm.type_logical);
}


SlagMethodInfo* SlagTypeInfo::find_method( const char* sig )
{
  for (int i=0; i<methods.count; ++i)
  {
    if (strcmp(methods[i]->signature,sig) == 0) return methods[i];
  }
  return NULL;
}

SlagMethodInfo* SlagTypeInfo::must_find_method( const char* sig )
{
  SlagMethodInfo* m = find_method(sig);
  if ( !m ) slag_throw_fatal_error( "Can't find method ", sig, "." );
  return m;
}

SlagPropertyInfo* SlagTypeInfo::find_property( const char* name, SlagTypeInfo* required_type )
{
  for (int i=0; i<properties.count; ++i)
  {
    SlagPropertyInfo* p = properties[i];
    if (strcmp(p->name,name) == 0) 
    {
      if (required_type && p->type != required_type)
      {
        slag_throw_fatal_error( "Property '", p->name, "' is not of the required type." );
      }
      return p;
    }
  }
  return NULL;
}

SlagPropertyInfo* SlagTypeInfo::must_find_property( const char* name, SlagTypeInfo* required_type )
{
  SlagPropertyInfo* p = find_property(name,required_type);
  if ( !p ) slag_throw_fatal_error( "Can't find property ", name, "." );
  return p;
}


SlagObject* SlagTypeInfo::singleton()
{
  return (SlagObject*) vm.singletons[singleton_index];
}

SlagObject* SlagTypeInfo::create()
{
  return (SlagObject*) mm.create_object(this, object_size);
}

SlagArray* SlagTypeInfo::create( int array_count )
{
  return mm.create_array(this,array_count);
}

//=============================================================================
// SlagMethodInfo
//=============================================================================
void SlagMethodInfo::create_signature()
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
//  SlagString
//=============================================================================
SlagTypeInfo* SlagString::string_type()
{
  return vm.type_string;
}

//=============================================================================
// Slag VM
//=============================================================================
void slag_assert( bool value, const char* st )
{
  if ( !value ) slag_throw_fatal_error( st );
}

void slag_assert( bool value, const char* st1, const char* st2, const char* st3 )
{
  if ( !value ) slag_throw_fatal_error( st1, st2, st3 );
}

void slag_throw_exception_of_type( SlagTypeInfo* type )
{
  slag_throw_fatal_error( "TODO: slag_throw_exception_of_type()" );
}

//=============================================================================
//  SlagVM
//=============================================================================
SlagVM::~SlagVM()
{
  shut_down();
}

void SlagVM::shut_down()
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

SlagObject* SlagVM::create_type( const char* name )
{
  SlagTypeInfo* type = must_find_type(name);
  return type->create();
}

SlagTypeInfo* SlagVM::find_type( const char* name )
{
  int count = vm.types.count;
  for (int i=0; i<count; ++i)
  {
    SlagTypeInfo* type = vm.types[i];
    if ( strcmp(type->name,name) == 0 ) return type;
  }
  return NULL;
}

SlagTypeInfo* SlagVM::must_find_type( const char* name )
{
  SlagTypeInfo* result = find_type(name);
  if (result) return result;
  return NULL;
}


void SlagVM::register_native_method( const char* class_name, const char* sig, SlagNativeFn fn )
{
  native_method_hooks.add( SlagNativeFnHook(class_name,sig,fn) );
}

static void Math__abs__Int64()
{
  SlagInt64 n = SLAG_POP_INTEGER();
  SLAG_DISCARD_REF();
  if (n < 0) n = -n;
  SLAG_PUSH_INTEGER(n);
}

static void Math__abs__Real64()
{
  SlagReal64 n = SLAG_POP_REAL();
  SLAG_DISCARD_REF();
  if (n < 0) n = -n;
  SLAG_PUSH_REAL(n);
}

static void Math__floor__Real64()
{
  SlagReal64 n = SLAG_POP_REAL();
  SLAG_DISCARD_REF();
  SLAG_PUSH_REAL(floor(n));
}

static void Math__sqrt__Real64()
{
  SlagReal64 n = SLAG_POP_REAL();
  SLAG_DISCARD_REF();
  SLAG_PUSH_REAL(sqrt(n));
}

static void Math__sin__Radians()
{
  SlagReal64 rads = SLAG_POP_REAL();
  SLAG_DISCARD_REF();
  SLAG_PUSH_REAL(sin(rads));
}

static void Math__cos__Radians()
{
  SlagReal64 rads = SLAG_POP_REAL();
  SLAG_DISCARD_REF();
  SLAG_PUSH_REAL(cos(rads));
}

static void Math__tan__Radians()
{
  SlagReal64 rads = SLAG_POP_REAL();
  SLAG_DISCARD_REF();
  SLAG_PUSH_REAL(tan(rads));
}

static void Math__asin__Real64()
{
  SlagReal64 n = SLAG_POP_REAL();
  SLAG_DISCARD_REF();
  SLAG_PUSH_REAL(asin(n));
}

static void Math__acos__Real64()
{
  SlagReal64 n = SLAG_POP_REAL();
  SLAG_DISCARD_REF();
  SLAG_PUSH_REAL(acos(n));
}

static void Math__atan__Real64()
{
  SlagReal64 n = SLAG_POP_REAL();
  SLAG_DISCARD_REF();
  SLAG_PUSH_REAL(atan(n));
}

static void Math__atan2__Real64_Real64()
{
  SlagReal64 x = SLAG_POP_REAL();
  SlagReal64 y = SLAG_POP_REAL();
  SLAG_DISCARD_REF();
  SLAG_PUSH_REAL(atan2(y,x));
}

/*
static void NativeData__clean_up()
{
  SlagNativeData* data = (SlagNativeData*) SLAG_POP_REF();
  data->release();
}


static void ParseReader__prep_data()
{
  ParseReader__prep_data( SLAG_POP_REF() );
}

static void ParseReader__has_another()
{
  SlagLogical result = ParseReader__has_another( (SlagParseReader*) SLAG_POP_REF() );
  SLAG_PUSH_INTEGER( result );
}

static void ParseReader__peek()
{
  SlagChar result = ParseReader__peek( (SlagParseReader*) SLAG_POP_REF() );
  SLAG_PUSH_INTEGER( result );
}

static void ParseReader__peek__Int32()
{
  SlagInt32 num_ahead = (SlagInt32) SLAG_POP_INTEGER();
  SlagChar result = ParseReader__peek__Int32( (SlagParseReader*) SLAG_POP_REF(), num_ahead );
  SLAG_PUSH_INTEGER( result );
}

static void ParseReader__read()
{
  SlagChar result = ParseReader__read( (SlagParseReader*) SLAG_POP_REF() );
  SLAG_PUSH_INTEGER( result );
}

static void ParseReader__consume__Char()
{
  SlagChar    ch = (SlagChar) SLAG_POP_INTEGER();
  SlagLogical result = ParseReader__consume__Char( (SlagParseReader*) SLAG_POP_REF(), ch );
  SLAG_PUSH_INTEGER( result );
}

static void ParseReader__consume__String()
{
  SlagString* st = (SlagString*) SLAG_POP_REF();
  SlagLogical result = ParseReader__consume__String( (SlagParseReader*) SLAG_POP_REF(), st );
  SLAG_PUSH_INTEGER( result );
}


*/

static bool find_ip_info( SlagInt64 ip, SlagMethodInfo** m_ptr, char** filename_ptr, int* line_ptr )
{
  *m_ptr = NULL;
  *line_ptr = 0;
  *filename_ptr = NULL;

  int offset = ((SlagOpcode*)(void*)ip) - vm.code.data;
  for (int i=0; i<vm.methods.count; ++i)
  {
    SlagMethodInfo* m = vm.methods[i];
    if (offset >= m->bytecode_offset && offset < m->bytecode_limit)
    {
      *m_ptr = m;
      break;
    }
  }

  if (*m_ptr == 0) return false;

  int best_filename_index = -1;
  int best_line = 0;
  for (int i=(*m_ptr)->source_pos_offset; i<vm.line_table.count; ++i)
  {
    SlagSourcePos pos = vm.line_table[i];
    if (pos.code_offset >= offset) break;
    if (pos.source_pos > 0) best_line = pos.source_pos;
    else best_filename_index = -pos.source_pos;
  }

  if (best_filename_index == -1) return false;

  *filename_ptr = vm.filenames[best_filename_index];
  *line_ptr = best_line;

  return true;
}

void StackTrace__describe__Int64()
{
  SlagInt64 ip = SLAG_POP_INTEGER();
  SLAG_DISCARD_REF();

  SlagMethodInfo* m;
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
    SLAG_PUSH_REF( (SlagObject*) SlagString::create(buffer.data,buffer.count) );
  }
  else
  {
    SLAG_PUSH_REF( (SlagObject*) SlagString::create("[internal]") );
  }
}

static void get_call_history( ArrayList<SlagInt64> &history )
{
  if (vm.exception_ip)
  {
    history.add( (SlagInt64)(void*)vm.exception_ip );
    vm.exception_ip = NULL;
  }

  SlagCallFrame* cur_frame = vm.regs.call_frame;
  while (cur_frame != vm.call_stack_limit)
  {
    history.add( (SlagInt64)(void*)((cur_frame++)->return_address) );
  }
  history.remove_last();
}

void StackTrace__native_history()
{
  SLAG_DISCARD_REF();
  ArrayList<SlagInt64> history;
  get_call_history(history);

  SlagArray* array = vm.type_array_of_int64->create( history.count );
  memcpy( array->data, history.data, history.count * 8 );
  SLAG_PUSH_REF( array );
}

/*
void StdInReader__native_read_char()
{
  SLAG_DISCARD_REF();
  SLAG_PUSH_INTEGER( getc(stdin) );
}

static void StdOutWriter__print__Char()
{
  StdOutWriter__print__Char( NULL, (SlagChar) SLAG_POP_INTEGER() );
  SLAG_DISCARD_REF();
}

static void StdOutWriter__print__String()
{
  StdOutWriter__print__String( NULL, (SlagString*) SLAG_POP_REF() );
  SLAG_DISCARD_REF();
}

static void StringBuilder__native_copy()
{
  SlagInt32 dest_offset = (SlagInt32) SLAG_POP_INTEGER();
  SlagArray*  dest_array = (SlagArray*) SLAG_POP_REF();
  SlagObject* src_string = SLAG_POP_REF();
  SLAG_DISCARD_REF();

  StringBuilder__native_copy__String_Array_of_Char_Int32( NULL, 
      src_string, dest_array, dest_offset );
}

static void StringManager__create_from__Array_of_Char_Int32()
{
  SlagInt32   count = (SlagInt32) SLAG_POP_INTEGER();
  SlagArray*  array = (SlagArray*) SLAG_POP_REF();
  SLAG_DISCARD_REF();
  SLAG_PUSH_REF( (SlagObject*) 
      StringManager__create_from__Array_of_Char_Int32(NULL,array,count) );
}

static void StringManager__create_from__Char()
{
  SlagChar chars[1];
  chars[0] = (SlagChar) SLAG_POP_INTEGER();
  SLAG_DISCARD_REF();
  SLAG_PUSH_REF( (SlagObject*) SlagString::create( chars, 1 ) );
}

static void String__count()
{
  SlagString* st = (SlagString*) SLAG_POP_REF();
  SLAG_PUSH_INTEGER( st->count );
}

static void String__hash_code()
{
  SlagString* st = (SlagString*) SLAG_POP_REF();
  SLAG_PUSH_INTEGER( st->hash_code );
}

static void String__get()
{
  SlagInt32 index = (SlagInt32) SLAG_POP_INTEGER();
  SlagString* st = (SlagString*) SLAG_POP_REF();
  SLAG_PUSH_INTEGER( st->characters[index] );
}

static void String__opCMP()
{
  SlagString* b = (SlagString*) SLAG_POP_REF();
  SlagString* a = (SlagString*) SLAG_POP_REF();
  SlagInt32 result = String__opCMP__String( a, b );
  SLAG_PUSH_INTEGER( result );
}

static void String__opEQ()
{
  SlagString* b = (SlagString*) SLAG_POP_REF();
  SlagString* a = (SlagString*) SLAG_POP_REF();
  SlagLogical result = String__opEQ__String( a, b );
  SLAG_PUSH_INTEGER( result );
}

static void String__opADD__String()
{
  SlagString* b = (SlagString*) SLAG_POP_REF();
  SlagString* a = (SlagString*) SLAG_POP_REF();
  SlagString* result = String__opADD__String( a, b );
  SLAG_PUSH_REF( (SlagObject*) result );
}

static void String__opADD__Char()
{
  SlagChar ch = (SlagChar) SLAG_POP_INTEGER();
  SlagString* st = (SlagString*) SLAG_POP_REF();
  SlagString* result = String__opADD__Char( st, ch );
  SLAG_PUSH_REF( (SlagObject*) result );
}

static void String__substring()
{
  SlagInt32 i2 = (SlagInt32) SLAG_POP_INTEGER();
  SlagInt32 i1 = (SlagInt32) SLAG_POP_INTEGER();
  SlagString* st = (SlagString*) SLAG_POP_REF();
  SlagString* result = String__substring__Int32_Int32( st, i1, i2 );
  SLAG_PUSH_REF( (SlagObject*) result );
}

static void String__to_Array()
{
  SlagObject* result = String__to_Array( (SlagString*) SLAG_POP_REF() );
  SLAG_PUSH_REF( (SlagObject*) result );
}

static void System__exit_program__Int32_String()
{
  SlagInt32   result_code = (SlagInt32) SLAG_POP_INTEGER();
  SlagString* mesg = (SlagString*) SLAG_POP_REF();
  if (mesg)
  {
    slag_error_message = mesg->to_new_ascii();
  }
  throw result_code;
}

static void System__get__String()
{
  SlagString* env_var_name = (SlagString*) SLAG_POP_REF();
  SLAG_DISCARD_REF();
  SlagString* result = System__get__String( NULL, env_var_name );
  SLAG_PUSH_REF( result );
}

static void System__os()
{
  SlagString* result = System__os( SLAG_POP_REF() );
  SLAG_PUSH_REF( result );
}

static void System__raw_exe_filepath()
{
  SLAG_DISCARD_REF();
  SlagString* result = System__raw_exe_filepath( NULL );
  SLAG_PUSH_REF( result );
}
*/

void SlagVM::register_stdlib()
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

void SlagVM::apply_hooks()
{
  for (int i=0; i<vm.native_method_hooks.count; ++i)
  {
    const char* class_name = vm.native_method_hooks[i].class_name;
    const char* sig = vm.native_method_hooks[i].signature;
    SlagNativeFn fn = vm.native_method_hooks[i].fn;
    SlagTypeInfo* type;
    SlagMethodInfo *m;
    type = vm.find_type( class_name );
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
          slag_throw_fatal_error( "Illegal specification of return type in native method hook." );
        }
      }
      continue;
    }
  }
  native_method_hooks.clear();
}


void SlagVM::set_command_line_args( char* args[], int count )
{
  SlagObject* list = ((SlagGenericObject*)(vm.must_find_type("Global")->singleton()))->get_ref("command_line_args");
  SlagMethodInfo* m_add = list->type->must_find_method("add(String)");
  for (int i=0; i<count; ++i)
  {
    SLAG_PUSH_REF( list );
    SLAG_PUSH_REF( (SlagObject*) SlagString::create(args[i]) );
    vm.call( m_add );
    SLAG_DISCARD_REF();
  }
}

void SlagVM::prep_types_and_methods()
{
  int num_singletons = 0;
  for (int t=0; t<vm.types.count; ++t)
  {
    SlagTypeInfo* type = vm.types[t];
	  
    if (type->qualifiers & SLAG_QUALIFIER_SINGLETON)
    {
      type->singleton_index = num_singletons++;
    }

    if (type->qualifiers & SLAG_QUALIFIER_PRIMITIVE)
    {
      type->stack_slots = 1;
      if (type == vm.type_int64)
      {
        type->object_size    = 8;
      }
      else if (type == vm.type_int32)
      {
        type->object_size    = 4;
      }
      else if (type == vm.type_char)
      {
        type->object_size    = 4;
      }
      else if (type == vm.type_byte)
      {
        type->object_size    = 4;
      }
      else if (type == vm.type_real64)
      {
        type->object_size    = 8;
      }
      else if (type == vm.type_real32)
      {
        type->object_size    = 4;
      }
      else if (type == vm.type_logical)
      {
        type->object_size    = 4;
      }
      else 
      {
        slag_throw_fatal_error( "Unknown primitive type." );
      }
    }
    else
    {
      if (type->is_reference())
      {
        type->object_size = sizeof(SlagObject);
        type->stack_slots = sizeof(void*);

        if (type->instance_of(vm.type_requires_cleanup) && type != vm.type_null)
        {
          type->qualifiers |= SLAG_QUALIFIER_REQUIRES_CLEANUP;
        }
      }
      else type->object_size = 0;

      for (int p=0; p<type->properties.count; ++p)
      {
        SlagPropertyInfo* property = type->properties[p];
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
              if (offsetof(SlagAlignmentInfo,real) == 8)
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
  vm.singletons.ensure_capacity( num_singletons );
  vm.singletons.count = num_singletons;

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
  for (int j=0; j<vm.parameter_table.count; ++j)
  {
    SlagParameterList& params = *vm.parameter_table[j];

    int   ref_offset = 0;
    int   data_offset = 0;

    --ref_offset; // for "this"
    ++params.num_ref_params;

    for (int i=0; i<params.count(); ++i)
    {
      SlagTypeInfo* type = params[i]->type;
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

  for (int j=0; j<vm.methods.count; ++j)
  {
    SlagMethodInfo* m = vm.methods[j];
    int ref_offset = -(m->parameters->num_ref_params);
    int data_offset = -(m->parameters->num_data_params);
    for (int i=0; i<m->local_vars.count; ++i)
    {
      SlagTypeInfo* type = m->local_vars[i]->type;
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
  for (int t=0; t<vm.types.count; ++t)
  {
    SlagTypeInfo* type = vm.types[t];

    type->method_init_object = type->find_method( "init_object()" );
    if (type->requires_cleanup())
    {
      type->method_clean_up = type->find_method( "clean_up()" );
      // Find instead of must_find - if a RequiresCleanup type doesn't have a clean_up()
      // method, that means no objects of that type are created.
    }
  }

  // Specific other methods & properties
  //vm.method_String_set_hash_code  = vm.type_string->must_find_method( "set_hash_code()" );
}


void SlagVM::configure()
{
  register_stdlib();

  apply_hooks();

  int ref_stack_count  = SLAG_STACK_SIZE;   // 8k
  int data_stack_count = SLAG_STACK_SIZE;
  int call_stack_count = SLAG_STACK_SIZE/2; // 4k

  if (ref_stack)
  {
    printf( "ERROR: ref_stack already initialized!\n" );
  }

  ref_stack = new SlagObject*[ref_stack_count];
  data_stack = new SlagInt64[data_stack_count];
  call_stack = new SlagCallFrame[call_stack_count];

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
  //for (int i=0; i<vm.strings.count; ++i)
  //{
    //SLAG_PUSH_REF( vm.strings[i] );
    //vm.call( vm.method_String_set_hash_code );
  //}

  create_singletons();

  /*
  vm_pass_command_line_args();
  */
}

void SlagVM::create_singletons()
{
  // Allocate memory for each singleton without calling
  // init_object() or init(). 
  for (int i=0; i<types.count; ++i)
  {
    SlagTypeInfo* type = types[i];
    if (type->is_singleton())
    {
      int index = type->singleton_index;
      singletons[index] = (mm.create_object( type, type->object_size ));
      ++singletons[index]->reference_count;
    }
  }

  // Create the main object without calling init_object() or init()
  SlagObject* main_obj;

  if (main_class->is_singleton())
  {
    main_obj = main_class->singleton();
  }
  else if (main_class->is_managed())
  {
    char buffer[512];
    sprintf( buffer, "%sManager", main_class->name );
    SLAG_FIND_TYPE( mgr_type, buffer );
    SlagObject* mgr_obj = mgr_type->singleton();

    main_obj = main_class->create();
    SLAG_SET_REF( mgr_obj, "singleton_instance", main_obj );
  }
  else
  {
    main_obj = main_class->create();
  }

  slag_main_object = main_obj;

  // Call Global.set_up_stdio() first so that any error messages
  // generated during general singleton instantatiation can 
  // be displayed.
  SlagTypeInfo* type_global = must_find_type( "Global" );
  SLAG_PUSH_REF( type_global->singleton() );
  SLAG_CALL( type_global, "set_up_stdio()" );

  // Call init_object() on each singleton.  Don't call
  // init_object() on the main object if it happens to
  // be a singleton.
  for (int i=0; i<vm.types.count; ++i)
  {
    SlagTypeInfo* type = vm.types[i];
    if (type->is_singleton() && type != vm.main_class)
    {
      if (type->method_init_object)
      {
        // Having only null references to unused singletons may 
        // result in a singleton that doesn't have an init_object() 
        // method.
        SLAG_PUSH_REF( type->singleton() );
        call( type->method_init_object );
      }
    }
  }

  // Call init_singleton() on each singleton if it exists.
  for (int i=0; i<vm.types.count; ++i)
  {
    SlagTypeInfo* type = vm.types[i];
    if (type->is_singleton())
    {
      SlagMethodInfo* m_init = type->find_method("init_singleton()");
      if (m_init)
      {
        SLAG_PUSH_REF( type->singleton()  );
        call( m_init );
      }
    }
  }
}

void SlagVM::call( SlagMethodInfo* method )
{
  static SlagOpcode halt_instruction = SLAGOP_HALT;

  if ( method )
  {
    SlagOpcode* prior_ip = vm.regs.ip;
    vm.regs.ip = &halt_instruction;

    invoke( method );
    execute();
    vm.regs.ip = prior_ip;
  }
  else
  {
    slag_throw_fatal_error( "Null method called." );
  }
}

void SlagVM::invoke( SlagMethodInfo* method )
{
  // Save prior call frame.
  (--vm.regs.call_frame)->return_address = vm.regs.ip;
  vm.regs.call_frame->called_method = method;
  vm.regs.call_frame->prior_frame = vm.regs.frame;

  // Set up current call frame pointers to be just before parameter data.
  vm.regs.frame.refs  = vm.regs.stack.refs  + method->parameters->num_ref_params;
  vm.regs.frame.data = vm.regs.stack.data + method->parameters->num_data_params;

  // Adjust stack pointers to make room for local data & clear that region.
  {
    int count = method->num_local_refs;
    if (count)
    {
      vm.regs.stack.refs -= count;
      memset( vm.regs.stack.refs, 0, count*sizeof(SlagObject*) );
      // need to clear refs so they don't screw up a GC
    }

    vm.regs.stack.data -= method->local_data_size;
    // Don't need to clear data since every local var is zeroed by default
    // if no other initial assignment is given.
  }
  vm.regs.ip = method->bytecode;
}


void SlagVM::execute()
{
  SlagMethodInfo *m = NULL;

  for (;;)
  {
    switch (*(vm.regs.ip++))
    {
      case SLAGOP_HALT:
        return;

      case SLAGOP_NOP:
        continue;

      case SLAGOP_BREAKPOINT:
        {
          SlagInt32 index = SLAGCODE_OPERAND_I32();
          printf( "Breakpoint %d\n", index );		
          continue;
        }       

      case SLAGOP_NATIVE_CALL:
        {
          m = vm.methods[SLAGCODE_OPERAND_I32()];

          // debug stack guard saves a lot of headache
          SlagOpcode* next_ip = vm.regs.ip;
          SlagObject** ref_stack = vm.regs.stack.refs + m->parameters->num_ref_params;
          SlagInt64* data_stack = vm.regs.stack.data + m->parameters->num_data_params;
          if (m->return_type)
          {
            if (m->return_type->is_reference()) --ref_stack;
            else data_stack -= m->return_type->stack_slots;
          }

          m->native_handler();

          if ((vm.regs.stack.refs != ref_stack || vm.regs.stack.data != data_stack)
              && vm.regs.ip == next_ip)
          {
            if (vm.regs.stack.refs != ref_stack) printf( "Corrupted reference stack" );
            else                                  printf( "Corrupted data stack" );
            printf( " in %s::%s\n",m->type_context->name,m->signature );
          }
        }
        continue;

      case SLAGOP_MISSING_RETURN_ERROR:
        VM_THROW_TYPE( vm.type_missing_return_error, continue );

      case SLAGOP_RETURN_NIL:
        vm.regs.stack = vm.regs.frame;
        vm.regs.frame = vm.regs.call_frame->prior_frame;
        vm.regs.ip = (vm.regs.call_frame++)->return_address;
        continue;

      case SLAGOP_RETURN_REF:
        {
          SlagObject* result  = SLAG_POP_REF();
          vm.regs.stack      = vm.regs.frame;
          vm.regs.frame      = vm.regs.call_frame->prior_frame;
          vm.regs.ip = (vm.regs.call_frame++)->return_address;
          SLAG_PUSH_REF( result );
        }
        continue;

      case SLAGOP_RETURN_8:
        {
          SlagInt64 result = SLAG_POP_INTEGER();
          vm.regs.stack   = vm.regs.frame;
          vm.regs.frame   = vm.regs.call_frame->prior_frame;
          vm.regs.ip = (vm.regs.call_frame++)->return_address;
          SLAG_PUSH_INTEGER( result );
          continue;
        }

      case SLAGOP_RETURN_X:
        {
          SlagInt64* from_spot = vm.regs.stack.data;
          SlagInt32 size  = SLAGCODE_OPERAND_I32();
          vm.regs.stack  = vm.regs.frame;
          vm.regs.frame  = vm.regs.call_frame->prior_frame;
          vm.regs.ip = (vm.regs.call_frame++)->return_address;
          vm.regs.stack.data -= size;
          memmove( vm.regs.stack.data, from_spot, size<<3 );
        }
        continue;

      case SLAGOP_DUPLICATE_REF:
        {
          SlagObject* ref = *vm.regs.stack.refs;
          SLAG_PUSH_REF( ref );
        }
        continue;

      case SLAGOP_DUPLICATE_8:
        {
          SlagInt64 n = *(vm.regs.stack.data);
          SLAG_PUSH_INTEGER( n );
          continue;
        }

      case SLAGOP_POP_REF:
        ++vm.regs.stack.refs;
        continue;

      case SLAGOP_POP_8:
        ++vm.regs.stack.data;
        continue;

      case SLAGOP_POP_X:
        {
          SlagInt32 size = SLAGCODE_OPERAND_I32();
          vm.regs.stack.data += size;
          continue;
        }

      case SLAGOP_JUMP:
        // Can't use OPERAND_ADDR() here since it would increment vm.regs.ip
        // after we assigned it.
        vm.regs.ip = (SlagOpcode*) vm.address_table[*vm.regs.ip];
        continue;

      case SLAGOP_JUMP_IF_TRUE:
        {
          SlagOpcode *dest = (SlagOpcode*) SLAGCODE_OPERAND_ADDR();
          if (SLAG_POP_INTEGER()>0) vm.regs.ip = dest;
        }
        continue;

      case SLAGOP_JUMP_IF_FALSE:
        {
          SlagOpcode *dest = (SlagOpcode*) SLAGCODE_OPERAND_ADDR();
          if (SLAG_POP_INTEGER()<=0) vm.regs.ip = dest;
        }
        continue;

      case SLAGOP_JUMP_IF_REF:
        {
          SlagOpcode *dest = (SlagOpcode*) SLAGCODE_OPERAND_ADDR();
          if (SLAG_POP_REF()) vm.regs.ip = dest;
        }
        continue;

      case SLAGOP_JUMP_IF_NULL_REF:
        {
          SlagOpcode *dest = (SlagOpcode*) SLAGCODE_OPERAND_ADDR();
          if ( !SLAG_POP_REF() ) vm.regs.ip = dest;
        }
        continue;

      case SLAGOP_THROW:
        throw_exception_on_stack();
        continue;

      case SLAGOP_LITERAL_STRING:
        SLAG_PUSH_REF( vm.strings[SLAGCODE_OPERAND_I32()] );
        continue;

      case SLAGOP_LITERAL_NULL:
        SLAG_PUSH_REF( NULL );
        continue;

      case SLAGOP_LITERAL_8:
        SLAG_PUSH_INTEGER( SLAGCODE_OPERAND_I64() );
        continue;

      case SLAGOP_LITERAL_4:
        SLAG_PUSH_INTEGER( SLAGCODE_OPERAND_I32() );
        continue;

      case SLAGOP_LITERAL_REAL32:
        {
          SlagInt32 n = SLAGCODE_OPERAND_I32();
          SLAG_PUSH_REAL( *((SlagReal32*)&n) );
        }
        continue;

      case SLAGOP_LITERAL_INTEGER_1:
        SLAG_PUSH_INTEGER( 1 );
        continue;

      case SLAGOP_LITERAL_INTEGER_0:
        SLAG_PUSH_INTEGER( 0 );
        continue;

      case SLAGOP_LITERAL_INTEGER_NEG1:
        SLAG_PUSH_INTEGER( -1 );
        continue;

      case SLAGOP_LITERAL_REAL_1:
        SLAG_PUSH_REAL( 1 );
        continue;

      case SLAGOP_LITERAL_REAL_0:
        SLAG_PUSH_REAL( 0 );
        continue;

      case SLAGOP_LITERAL_REAL_NEG1:
        SLAG_PUSH_REAL( -1 );
        continue;

      case SLAGOP_READ_THIS_REF:
        SLAG_PUSH_REF( vm.regs.frame.refs[-1] );
        continue;

      case SLAGOP_READ_SINGLETON_REF:
        SLAG_PUSH_REF( vm.singletons[SLAGCODE_OPERAND_I32()] );
        continue;

      case SLAGOP_READ_PROPERTY_REF:
        {
          SlagObject* context = SLAG_POP_REF( );
          VM_NULL_CHECK( context, continue );
          SLAG_PUSH_REF( VM_DEREFERENCE(context,SLAGCODE_OPERAND_I32(),SlagObject*) );
        }
        continue;

      case SLAGOP_READ_PROPERTY_1U:
        SLAGCODE_READ_PROPERTY( SlagByte );
        continue;

      case SLAGOP_READ_PROPERTY_2U:
        SLAGCODE_READ_PROPERTY( SlagChar );
        continue;

      case SLAGOP_READ_PROPERTY_4:
        SLAGCODE_READ_PROPERTY( SlagInt32 );
        continue;

      case SLAGOP_READ_PROPERTY_REAL32:
        {
          SlagObject* context = SLAG_POP_REF();
          VM_ASSERT( context, vm.type_null_reference_error, continue );
          SLAG_PUSH_REAL( VM_DEREFERENCE(context,SLAGCODE_OPERAND_I32(),SlagReal32) );
        }
        continue;

      case SLAGOP_READ_PROPERTY_8:
        SLAGCODE_READ_PROPERTY( SlagInt64 );
        continue;

      case SLAGOP_READ_PROPERTY_X:
        {
          int offset, size;
          SlagObject* context = SLAG_POP_REF();
          VM_NULL_CHECK( context, continue );
          offset = SLAGCODE_OPERAND_I32();
          size = SLAGCODE_OPERAND_I32();
          vm.regs.stack.data -= size>>3;
          memcpy( vm.regs.stack.data, ((char*)context)+offset, size );
        }
        continue;

      case SLAGOP_READ_THIS_PROPERTY_REF:
        SLAG_PUSH_REF( VM_DEREFERENCE(vm.regs.frame.refs[-1],SLAGCODE_OPERAND_I32(),SlagObject*) );
        continue;

      case SLAGOP_READ_THIS_PROPERTY_4:
        SLAG_PUSH_INTEGER( VM_DEREFERENCE(vm.regs.frame.refs[-1],SLAGCODE_OPERAND_I32(),SlagInt32) );
        continue;

      case SLAGOP_READ_THIS_PROPERTY_REAL32:
        SLAG_PUSH_REAL( VM_DEREFERENCE(vm.regs.frame.refs[-1],SLAGCODE_OPERAND_I32(),SlagReal32) );
        continue;

      case SLAGOP_READ_THIS_PROPERTY_8:
        SLAG_PUSH_INTEGER( VM_DEREFERENCE(vm.regs.frame.refs[-1],SLAGCODE_OPERAND_I32(),SlagInt64) );
        continue;

      case SLAGOP_READ_THIS_PROPERTY_X:
        {
          SlagInt32 offset = SLAGCODE_OPERAND_I32();
          SlagInt32 size = SLAGCODE_OPERAND_I32();
          vm.regs.stack.data -= size>>3;
          memcpy( vm.regs.stack.data, ((char*)vm.regs.frame.refs[-1])+offset, size );
        }
        continue;

      case SLAGOP_READ_LOCAL_REF:
        SLAG_PUSH_REF( vm.regs.frame.refs[SLAGCODE_OPERAND_I32()] );
        continue;

      case SLAGOP_READ_LOCAL_8:
        SLAG_PUSH_INTEGER( vm.regs.frame.data[SLAGCODE_OPERAND_I32()] );
        continue;

      case SLAGOP_READ_LOCAL_X:
        {
          SlagInt32 offset = (SlagInt32) SLAGCODE_OPERAND_I32();
          SlagInt32 slots  = (SlagInt32) SLAGCODE_OPERAND_I32();
          vm.regs.stack.data -= slots;
          memcpy( vm.regs.stack.data, vm.regs.frame.data+offset, slots<<3 );
          continue;
        }

      case SLAGOP_READ_COMPOUND_8:
        {
          SlagInt32 total_slots = SLAGCODE_OPERAND_I32();
          SlagInt32 offset     = SLAGCODE_OPERAND_I32();
          SlagInt64 i64 = vm.regs.stack.data[offset];
          vm.regs.stack.data += total_slots;
          SLAG_PUSH_INTEGER( i64 );
        }
        continue;

      case SLAGOP_READ_COMPOUND_X:
        {
          SlagInt32 total_slots = SLAGCODE_OPERAND_I32();
          SlagInt32 offset      = SLAGCODE_OPERAND_I32();
          SlagInt32 slots       = SLAGCODE_OPERAND_I32();
          SlagInt64* source_ptr = vm.regs.stack.data + offset;
          vm.regs.stack.data += (total_slots - slots);
          memmove( vm.regs.stack.data, source_ptr, slots<<3 );
        }
        continue;

      case SLAGOP_WRITE_SINGLETON_REF:
        {
          SlagObject*  new_value = SLAG_POP_REF();
          SlagObject** dest = vm.singletons.data + SLAGCODE_OPERAND_I32();
          if (*dest) --((*dest)->reference_count);
          if (new_value) ++(new_value->reference_count);
          *dest = new_value;
        }
        continue;

      case SLAGOP_WRITE_PROPERTY_REF:
        {
          SlagObject* new_value = SLAG_POP_REF();
          SlagObject* context = SLAG_POP_REF();
          VM_NULL_CHECK( context, continue );
          SlagInt32   offset = SLAGCODE_OPERAND_I32();
          SlagObject** dest = (SlagObject**)(((char*)context)+offset);
          if (*dest) --((*dest)->reference_count);
          if (new_value) ++(new_value->reference_count);
          *dest = new_value;
        }
        continue;

      case SLAGOP_WRITE_PROPERTY_1:
        SLAGCODE_WRITE_PROPERTY( SlagInt8 );
        continue;

      case SLAGOP_WRITE_PROPERTY_2:
        SLAGCODE_WRITE_PROPERTY( SlagInt16 );
        continue;

      case SLAGOP_WRITE_PROPERTY_4:
        SLAGCODE_WRITE_PROPERTY( SlagInt32 );
        continue;

      case SLAGOP_WRITE_PROPERTY_REAL32:
        {
          SlagReal64 new_value = SLAG_POP_REAL();
          SlagObject* context = SLAG_POP_REF();
          VM_ASSERT( context, vm.type_null_reference_error, continue );
          VM_DEREFERENCE(context,SLAGCODE_OPERAND_I32(),SlagReal32) = (SlagReal32) new_value;
        }
        continue;

      case SLAGOP_WRITE_PROPERTY_8:
        SLAGCODE_WRITE_PROPERTY( SlagInt64 );
        continue;

      case SLAGOP_WRITE_PROPERTY_X:
        {
          SlagInt32 offset = SLAGCODE_OPERAND_I32();
          SlagInt32 size   = SLAGCODE_OPERAND_I32();
          SlagObject* context = SLAG_POP_REF();
          VM_NULL_CHECK( context, continue );
          memcpy( ((char*)context)+offset, vm.regs.stack.data, size );
          vm.regs.stack.data += size>>3;
        }
        continue;

      case SLAGOP_WRITE_THIS_PROPERTY_REF:
        {
          SlagObject* new_value = SLAG_POP_REF();
          SlagInt32 offset = SLAGCODE_OPERAND_I32();
          VM_DEREFERENCE(vm.regs.frame.refs[-1],offset,SlagObject*) = new_value;

          SlagObject** dest = (SlagObject**)(((char*)vm.regs.frame.refs[-1])+offset);
          if (*dest) --((*dest)->reference_count);
          if (new_value) ++(new_value->reference_count);
          *dest = new_value;
        }
        continue;

      case SLAGOP_WRITE_THIS_PROPERTY_4:
        SLAGCODE_WRITE_THIS_PROPERTY( SlagInt32 );
        continue;

      case SLAGOP_WRITE_THIS_PROPERTY_REAL32:
        VM_DEREFERENCE(vm.regs.frame.refs[-1],SLAGCODE_OPERAND_I32(),SlagReal32) = (SlagReal32) SLAG_POP_REAL();
        continue;

      case SLAGOP_WRITE_THIS_PROPERTY_8:
        SLAGCODE_WRITE_THIS_PROPERTY( SlagInt64 );
        continue;

      case SLAGOP_WRITE_THIS_PROPERTY_X:
        {
          SlagInt32 offset = SLAGCODE_OPERAND_I32();
          SlagInt32 size   = SLAGCODE_OPERAND_I32();
          memcpy( ((char*)vm.regs.frame.refs[-1])+offset, vm.regs.stack.data, size );
          vm.regs.stack.data += size>>3;
        }
        continue;

      case SLAGOP_WRITE_LOCAL_REF:
        vm.regs.frame.refs[SLAGCODE_OPERAND_I32()] = SLAG_POP_REF();
        continue;

      case SLAGOP_WRITE_LOCAL_8:
        vm.regs.frame.data[SLAGCODE_OPERAND_I32()] = SLAG_POP_INTEGER();
        continue;

      case SLAGOP_WRITE_LOCAL_X:
        {
          SlagInt32 offset = SLAGCODE_OPERAND_I32();
          SlagInt32 slots  = SLAGCODE_OPERAND_I32();
          memcpy( vm.regs.frame.data+offset, vm.regs.stack.data, slots<<3 );
          vm.regs.stack.data += slots;
        }
        continue;

      case SLAGOP_FAUX_STATIC_CALL:
        // Args have been evaluated & pushed on the stack but we don't
        // actually make the call since it's an empty method - we just
        // remove all the stuff on the stack again.
        m = vm.methods[SLAGCODE_OPERAND_I32()];
        vm.regs.stack.refs  += m->parameters->num_ref_params;
        vm.regs.stack.data += m->parameters->num_data_params;
        continue;

      case SLAGOP_STATIC_CALL:
        m = vm.methods[SLAGCODE_OPERAND_I32()];
        VM_NULL_CHECK( vm.regs.stack.refs[SLAGCODE_OPERAND_I32()], continue );
        goto execute_call;

      case SLAGOP_DYNAMIC_CALL:
        {
          SlagObject* context = vm.regs.stack.refs[ SLAGCODE_OPERAND_I32() ];
          if (context) 
          {
            m = context->type->dispatch_table[ SLAGCODE_OPERAND_I32() ];
            goto execute_call;
          }
          else
          {
            VM_THROW_TYPE( vm.type_null_reference_error, continue );
            continue;
          }
        }

      case SLAGOP_NEW_OBJECT:
        {
          SlagTypeInfo* of_type = vm.types[ SLAGCODE_OPERAND_I32() ];
          SlagObject* object = mm.create_object( of_type, of_type->object_size );
          SLAG_PUSH_REF( object ); // first ref is result value
          SLAG_PUSH_REF( object ); // second ref is for init() call
          SLAG_PUSH_REF( object ); // third ref is for init_object() call
          m = of_type->method_init_object;
          goto execute_call;
        }

      case SLAGOP_NEW_OBJECT_NO_INIT:
        {
          SlagTypeInfo* of_type = vm.types[ SLAGCODE_OPERAND_I32() ];
          SlagObject* object = mm.create_object( of_type, of_type->object_size );
          SLAG_PUSH_REF( object ); // first ref is result value
          // we skip init_object() call
          SLAG_PUSH_REF( object ); // second ref is for following init() call
          continue;
        }

      case SLAGOP_NEW_ARRAY:
        {
          SlagTypeInfo* of_type = vm.types[ SLAGCODE_OPERAND_I32() ];
          SlagInt32 size = (SlagInt32) SLAG_POP_INTEGER();
          if (size < 0) VM_THROW_TYPE( vm.type_invalid_operand_error, continue );
          SLAG_PUSH_REF( mm.create_array(of_type,size) );
          continue;
        }

      case SLAGOP_ARRAY_READ_REF:
        {
          SlagArray* array_ref = (SlagArray*) SLAG_POP_REF();
          SlagInt32   index = (SlagInt32) SLAG_POP_INTEGER();
          if (!array_ref) VM_THROW_TYPE(vm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            VM_THROW_TYPE( vm.type_out_of_bounds_error, continue );
          }
          SLAG_PUSH_REF( ((SlagArray**)array_ref->data)[index] );
          continue;
        }

      case SLAGOP_ARRAY_READ_1U:
        {
          SlagArray* array_ref = (SlagArray*) SLAG_POP_REF();
          SlagInt32   index = (SlagInt32) SLAG_POP_INTEGER();
          if (!array_ref) VM_THROW_TYPE(vm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            VM_THROW_TYPE( vm.type_out_of_bounds_error, continue );
          }
          SLAG_PUSH_INTEGER( ((SlagByte*)array_ref->data)[index] );
          continue;
        }

      case SLAGOP_ARRAY_READ_2U:
        {
          SlagArray* array_ref = (SlagArray*) SLAG_POP_REF();
          SlagInt32   index = (SlagInt32) SLAG_POP_INTEGER();
          if (!array_ref) VM_THROW_TYPE(vm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            VM_THROW_TYPE( vm.type_out_of_bounds_error, continue );
          }
          SLAG_PUSH_INTEGER( ((SlagChar*)array_ref->data)[index] );
          continue;
        }

      case SLAGOP_ARRAY_READ_4:
        {
          SlagArray* array_ref = (SlagArray*) SLAG_POP_REF();
          SlagInt32   index = (SlagInt32) SLAG_POP_INTEGER();
          if (!array_ref) VM_THROW_TYPE(vm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            VM_THROW_TYPE( vm.type_out_of_bounds_error, continue );
          }
          SLAG_PUSH_INTEGER( ((SlagInt32*)array_ref->data)[index] );
          continue;
        }

      case SLAGOP_ARRAY_READ_REAL32:
        {
          SlagArray* array_ref = (SlagArray*) SLAG_POP_REF();
          SlagInt32      index = (SlagInt32) SLAG_POP_INTEGER();
          if (!array_ref) VM_THROW_TYPE(vm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            VM_THROW_TYPE( vm.type_out_of_bounds_error, continue );
          }
          SLAG_PUSH_REAL( ((SlagReal32*)array_ref->data)[index] );
          continue;
        }

      case SLAGOP_ARRAY_READ_8:
        {
          SlagArray* array_ref = (SlagArray*) SLAG_POP_REF();
          SlagInt32   index = (SlagInt32) SLAG_POP_INTEGER();
          if (!array_ref) VM_THROW_TYPE(vm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            VM_THROW_TYPE( vm.type_out_of_bounds_error, continue );
          }
          SLAG_PUSH_INTEGER( ((SlagInt64*)array_ref->data)[index] );
          continue;
        }

      case SLAGOP_ARRAY_READ_X:
        {
          SlagArray* array_ref = (SlagArray*) SLAG_POP_REF();
          SlagInt32 index = (SlagInt32) SLAG_POP_INTEGER();
          VM_NULL_CHECK( array_ref, continue );
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            VM_THROW_TYPE( vm.type_out_of_bounds_error, continue );
          }
          int size = array_ref->type->element_size;
          memcpy( (vm.regs.stack.data -= size>>3), ((char*)array_ref->data)+index*size, size );
          continue;
        }

      case SLAGOP_ARRAY_WRITE_REF:
        {
          SlagObject* new_value = SLAG_POP_REF();
          SlagArray* array_ref = (SlagArray*) SLAG_POP_REF();
          SlagInt32   index = (SlagInt32) SLAG_POP_INTEGER();
          if (!array_ref) VM_THROW_TYPE(vm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            VM_THROW_TYPE( vm.type_out_of_bounds_error,continue );
          }
          SlagObject** dest = ((SlagObject**)array_ref->data) + index;
          if (*dest) --((*dest)->reference_count);
          if (new_value) ++new_value->reference_count;
          *dest = new_value;
          continue;
        }

      case SLAGOP_ARRAY_WRITE_1:
        {
          SlagInt64   new_value = SLAG_POP_INTEGER();
          SlagArray* array_ref = (SlagArray*) SLAG_POP_REF();
          SlagInt32   index = (SlagInt32) SLAG_POP_INTEGER();
          if (!array_ref) VM_THROW_TYPE(vm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            VM_THROW_TYPE( vm.type_out_of_bounds_error,continue );
          }
          ((SlagByte*)array_ref->data)[index] = (SlagByte) new_value;
          continue;
        }

      case SLAGOP_ARRAY_WRITE_2:
        {
          SlagInt64   new_value = SLAG_POP_INTEGER();
          SlagArray* array_ref = (SlagArray*) SLAG_POP_REF();
          SlagInt32   index = (SlagInt32) SLAG_POP_INTEGER();
          if (!array_ref) VM_THROW_TYPE(vm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            VM_THROW_TYPE( vm.type_out_of_bounds_error, continue );
          }
          ((SlagChar*)array_ref->data)[index] = (SlagChar) new_value;
          continue;
        }

      case SLAGOP_ARRAY_WRITE_4:
        {
          SlagInt64   new_value = SLAG_POP_INTEGER();
          SlagArray* array_ref = (SlagArray*) SLAG_POP_REF();
          SlagInt32   index = (SlagInt32) SLAG_POP_INTEGER();
          if (!array_ref) VM_THROW_TYPE(vm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            VM_THROW_TYPE( vm.type_out_of_bounds_error, continue );
          }
          ((SlagInt32*)array_ref->data)[index] = (SlagInt32) new_value;
          continue;
        }

      case SLAGOP_ARRAY_WRITE_REAL32:
        {
          SlagReal64 new_value = SLAG_POP_REAL();
          SlagArray* array_ref = (SlagArray*) SLAG_POP_REF();
          SlagInt32   index = (SlagInt32) SLAG_POP_INTEGER();
          if (!array_ref) VM_THROW_TYPE(vm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            VM_THROW_TYPE( vm.type_out_of_bounds_error, continue );
          }
          ((SlagReal32*)array_ref->data)[index] = (SlagReal32) new_value;
          continue;
        }

      case SLAGOP_ARRAY_WRITE_8:
        {
          SlagInt64   new_value = SLAG_POP_INTEGER();
          SlagArray* array_ref = (SlagArray*) SLAG_POP_REF();
          SlagInt32   index = (SlagInt32) SLAG_POP_INTEGER();
          if (!array_ref) VM_THROW_TYPE(vm.type_null_reference_error,continue);
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            VM_THROW_TYPE( vm.type_out_of_bounds_error, continue );
          }
          ((SlagInt64*)array_ref->data)[index] = (SlagInt64) new_value;
          continue;
        }

      case SLAGOP_ARRAY_WRITE_X:
        {
          SlagArray* array_ref = (SlagArray*) SLAG_POP_REF();
          VM_NULL_CHECK( array_ref, continue );
          int size = array_ref->type->element_size;
          char* source_ptr = (char*) vm.regs.stack.data;
          vm.regs.stack.data += size >> 3;
          SlagInt32 index = (SlagInt32) SLAG_POP_INTEGER();
          if ((unsigned int) index >= (unsigned int) array_ref->array_count)
          {
            VM_THROW_TYPE( vm.type_out_of_bounds_error, continue );
          }
          memcpy( ((char*)array_ref->data)+index*size, source_ptr, size );
          continue;
        }

      case SLAGOP_ARRAY_DUPLICATE:
        {
          SlagObject* array = SLAG_POP_REF();
          SLAG_PUSH_REF( slag_duplicate_array(array) );
          continue;
        }

      case SLAGOP_TYPECHECK:
        {
          SlagTypeInfo* of_type = vm.types[ SLAGCODE_OPERAND_I32() ];
          SlagObject* object = SLAG_PEEK_REF();
          if (object && !object->type->instance_of(of_type)) 
          {
            VM_THROW_TYPE( vm.type_type_cast_error, continue );
          }
          continue;
        }

      case SLAGOP_AS_REF:
        {
          SlagTypeInfo* of_type = vm.types[ SLAGCODE_OPERAND_I32() ];
          SlagObject* object = SLAG_PEEK_REF();
          if (object && !object->type->instance_of(of_type)) SLAG_PEEK_REF() = NULL;
          continue;
        }

      case SLAGOP_CAST_REAL_TO_INTEGER:
        {
          SlagReal64 value = *((SlagReal64*)vm.regs.stack.data);
          *vm.regs.stack.data = (SlagInt64) value;
        }
        continue;

      case SLAGOP_CAST_INTEGER_TO_REAL:
        {
          SlagInt64 value = *vm.regs.stack.data;
          *((SlagReal64*)vm.regs.stack.data) = (SlagReal64) value;
        }
        continue;

      case SLAGOP_CAST_REAL_TO_LOGICAL:
        {
          SlagReal64 value = *((SlagReal64*)vm.regs.stack.data);
          if (value != 0) *vm.regs.stack.data = 1;
          else            *vm.regs.stack.data = 0;
        }
        continue;

      case SLAGOP_CAST_INTEGER_TO_LOGICAL:
        {
          SlagInt64 value = *vm.regs.stack.data;
          if (value != 0) *vm.regs.stack.data = 1;
          else            *vm.regs.stack.data = 0;
        }
        continue;

      case SLAGOP_CAST_INTEGER_TO_I32:
        *vm.regs.stack.data = (SlagInt32) *vm.regs.stack.data;
        continue;

      case SLAGOP_CAST_INTEGER_TO_CHAR:
        *vm.regs.stack.data &= 0xffff;
        continue;

      case SLAGOP_CAST_INTEGER_TO_BYTE:
        *vm.regs.stack.data &= 0xff;
        continue;

      case SLAGOP_CMP_INSTANCE_OF:
        {
          SlagTypeInfo* of_type = vm.types[ SLAGCODE_OPERAND_I32() ];
          SlagObject* object = SLAG_POP_REF();
          if (object) SLAG_PUSH_INTEGER( object->type->instance_of(of_type));
          else SLAG_PUSH_INTEGER( 0 );
        }
        continue;

      case SLAGOP_CMP_EQ_REF:
        {
          SlagObject* b = SLAG_POP_REF();
          SlagObject* a = SLAG_POP_REF();
          SLAG_PUSH_INTEGER( a == b);
        }
        continue;

      case SLAGOP_CMP_NE_REF:
        {
          SlagObject* b = SLAG_POP_REF();
          SlagObject* a = SLAG_POP_REF();
          SLAG_PUSH_INTEGER( a != b);
        }
        continue;

      case SLAGOP_CMP_EQ_REF_NULL:
        {
          SlagObject* obj = SLAG_POP_REF();
          SLAG_PUSH_INTEGER( obj == NULL );
        }
        continue;

      case SLAGOP_CMP_NE_REF_NULL:
        {
          SlagObject* obj = SLAG_POP_REF();
          SLAG_PUSH_INTEGER( obj != NULL );
        }
        continue;

      case SLAGOP_CMP_EQ_X:
        {
          SlagInt32 result = 1;
          SlagInt32 slots = SLAGCODE_OPERAND_I32();
          
          SlagInt64* ptr1 = (SlagInt64*) vm.regs.stack.data;
          vm.regs.stack.data += slots;
          SlagInt64* ptr2 = (SlagInt64*) vm.regs.stack.data;
          vm.regs.stack.data += slots;

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

          SLAG_PUSH_INTEGER( result );
        }
        continue;

      case SLAGOP_CMP_NE_X:
        {
          SlagInt32 result = 0;
          SlagInt32 slots = SLAGCODE_OPERAND_I32();
          
          SlagInt64* ptr1 = (SlagInt64*) vm.regs.stack.data;
          vm.regs.stack.data += slots;
          SlagInt64* ptr2 = (SlagInt64*) vm.regs.stack.data;
          vm.regs.stack.data += slots;

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

          SLAG_PUSH_INTEGER( result );
        }
        continue;

      case SLAGOP_CMP_EQ_INTEGER:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a == b);
        }
        continue;

      case SLAGOP_CMP_NE_INTEGER:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a != b);
        }
        continue;

      case SLAGOP_CMP_GT_INTEGER:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a > b);
        }
        continue;

      case SLAGOP_CMP_GE_INTEGER:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a >= b);
        }
        continue;

      case SLAGOP_CMP_LT_INTEGER:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a < b);
        }
        continue;

      case SLAGOP_CMP_LE_INTEGER:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a <= b);
        }
        continue;

      case SLAGOP_CMP_EQ_REAL:
        {
          SlagReal64 b = SLAG_POP_REAL();
          SlagReal64 a = SLAG_PEEK_REAL();
          SLAG_PEEK_INTEGER() = (a == b);
        }
        continue;

      case SLAGOP_CMP_NE_REAL:
        {
          SlagReal64 b = SLAG_POP_REAL();
          SlagReal64 a = SLAG_PEEK_REAL();
          SLAG_PEEK_INTEGER() = (a != b);
        }
        continue;

      case SLAGOP_CMP_GT_REAL:
        {
          SlagReal64 b = SLAG_POP_REAL();
          SlagReal64 a = SLAG_PEEK_REAL();
          SLAG_PEEK_INTEGER() = (a > b);
        }
        continue;

      case SLAGOP_CMP_GE_REAL:
        {
          SlagReal64 b = SLAG_POP_REAL();
          SlagReal64 a = SLAG_PEEK_REAL();
          SLAG_PEEK_INTEGER() = (a >= b);
        }
        continue;

      case SLAGOP_CMP_LT_REAL:
        {
          SlagReal64 b = SLAG_POP_REAL();
          SlagReal64 a = SLAG_PEEK_REAL();
          SLAG_PEEK_INTEGER() = (a < b);
        }
        continue;

      case SLAGOP_CMP_LE_REAL:
        {
          SlagReal64 b = SLAG_POP_REAL();
          SlagReal64 a = SLAG_PEEK_REAL();
          SLAG_PEEK_INTEGER() = (a <= b);
        }
        continue;

      case SLAGOP_NOT_INTEGER:
        *vm.regs.stack.data = ~(*vm.regs.stack.data);
        continue;

      case SLAGOP_NOT_LOGICAL:
        // Logical values can be -1, 0, or 1, so a little extra attention is required.
        if (*vm.regs.stack.data > 0) *vm.regs.stack.data = 0;
        else                          *vm.regs.stack.data = 1;
        continue;

      case SLAGOP_NEGATE_INTEGER:
        *vm.regs.stack.data = -(*vm.regs.stack.data);
        continue;

      case SLAGOP_NEGATE_REAL:
        *((SlagReal64*)vm.regs.stack.data) = -(*((SlagReal64*)vm.regs.stack.data));
        continue;

      case SLAGOP_AND_LOGICAL:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a & b);
        }
        continue;

      case SLAGOP_OR_LOGICAL:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a | b);
        }
        continue;

      case SLAGOP_XOR_LOGICAL:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a ^ b);
        }
        continue;

      case SLAGOP_ADD_INTEGER:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a + b);
        }
        continue;

      case SLAGOP_SUB_INTEGER:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a - b);
        }
        continue;

      case SLAGOP_MUL_INTEGER:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a * b);
        }
        continue;

      case SLAGOP_DIV_INTEGER:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          VM_ASSERT( b, vm.type_divide_by_zero_error, continue );
          SLAG_PEEK_INTEGER() = (a / b);
        }
        continue;

      case SLAGOP_MOD_INTEGER:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          VM_ASSERT( b, vm.type_divide_by_zero_error, continue );
          if (b == 1)
          {
            SLAG_PEEK_INTEGER() = 0;
          }
          else if ((a ^ b) < 0)
          {
            SlagInt64 r = a % b;
            SLAG_PEEK_INTEGER() = (r ? (r+b) : r);
          }
          else
          {
            SLAG_PEEK_INTEGER() = (a % b);
          }
        }
        continue;

      case SLAGOP_EXP_INTEGER:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (SlagInt64) pow((double)a,(double)b);
        }
        continue;

      case SLAGOP_AND_INTEGER:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a & b);
        }
        continue;

      case SLAGOP_OR_INTEGER:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a | b);
        }
        continue;

      case SLAGOP_XOR_INT64:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a ^ b);
        }
        continue;

      case SLAGOP_SHL_INT64:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a << b);
        }
        continue;

      case SLAGOP_SHR_INT64:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          if (b == 0) continue;

          SlagInt64 a = SLAG_PEEK_INTEGER();
          a = (a >> 1) & 0x7fffFFFFffffFFFFLL;
          if (--b > 0) a >>= b;

          SLAG_PEEK_INTEGER() = a;
        }
        continue;

      case SLAGOP_SHRX_INT64:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a >> b);
        }
        continue;

      case SLAGOP_XOR_INT32:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (SlagInt32)(a ^ b);
        }
        continue;

      case SLAGOP_SHL_INT32:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          SlagInt64 a = SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (SlagInt32)(a << b);
        }
        continue;

      case SLAGOP_SHR_INT32:
        {
          SlagInt64 b = SLAG_POP_INTEGER();
          if (b == 0) continue;

          SlagInt64 a = SLAG_PEEK_INTEGER();
          a = (a >> 1) & 0x7fffFFFF;
          if (--b > 0) a >>= b;
          SLAG_PEEK_INTEGER() = a;
        }
        continue;

      case SLAGOP_SHRX_INT32:
        {
          SlagInt32 b = (SlagInt32) SLAG_POP_INTEGER();
          SlagInt32 a = (SlagInt32) SLAG_PEEK_INTEGER();
          SLAG_PEEK_INTEGER() = (a >> b);
        }
        continue;

      case SLAGOP_ADD_REAL:
        {
          SlagReal64 b = SLAG_POP_REAL();
          SlagReal64 a = SLAG_PEEK_REAL();
          SLAG_PEEK_REAL() = (a + b);
        }
        continue;

      case SLAGOP_SUB_REAL:
        {
          SlagReal64 b = SLAG_POP_REAL();
          SlagReal64 a = SLAG_PEEK_REAL();
          SLAG_PEEK_REAL() = (a - b);
        }
        continue;

      case SLAGOP_MUL_REAL:
        {
          SlagReal64 b = SLAG_POP_REAL();
          SlagReal64 a = SLAG_PEEK_REAL();
          SLAG_PEEK_REAL() = (a * b);
        }
        continue;

      case SLAGOP_DIV_REAL:
        {
          SlagReal64 b = SLAG_POP_REAL();
          SlagReal64 a = SLAG_PEEK_REAL();
          SLAG_PEEK_REAL() = (a / b);
        }
        continue;

      case SLAGOP_MOD_REAL:
        {
          SlagReal64 b = SLAG_POP_REAL();
          SlagReal64 a = SLAG_PEEK_REAL();
          SlagReal64 q = a / b;
          SLAG_PEEK_REAL() = (SlagReal64) (a - (floor(q)) * b);
        }
        continue;

      case SLAGOP_EXP_REAL:
        {
          SlagReal64 b = SLAG_POP_REAL();
          SlagReal64 a = SLAG_PEEK_REAL();
          SLAG_PEEK_REAL() = pow(a,b);
        }
        continue;

      case SLAGOP_THIS_ADD_ASSIGN_I32:
        VM_DEREFERENCE(vm.regs.frame.refs[-1],SLAGCODE_OPERAND_I32(),SlagInt32) += (SlagInt32) SLAG_POP_INTEGER();
        continue;

      case SLAGOP_THIS_SUB_ASSIGN_I32:
        VM_DEREFERENCE(vm.regs.frame.refs[-1],SLAGCODE_OPERAND_I32(),SlagInt32) -= (SlagInt32) SLAG_POP_INTEGER();
        continue;

      case SLAGOP_THIS_MUL_ASSIGN_I32:
        VM_DEREFERENCE(vm.regs.frame.refs[-1],SLAGCODE_OPERAND_I32(),SlagInt32) *= (SlagInt32) SLAG_POP_INTEGER();
        continue;

      case SLAGOP_THIS_INCREMENT_I32:
        ++VM_DEREFERENCE(vm.regs.frame.refs[-1],SLAGCODE_OPERAND_I32(),SlagInt32);
        continue;

      case SLAGOP_THIS_DECREMENT_I32:
        --VM_DEREFERENCE(vm.regs.frame.refs[-1],SLAGCODE_OPERAND_I32(),SlagInt32);
        continue;

      case SLAGOP_THIS_ADD_ASSIGN_R64:
        VM_DEREFERENCE(vm.regs.frame.refs[-1],SLAGCODE_OPERAND_I32(),SlagReal64) += SLAG_POP_REAL();
        continue;

      case SLAGOP_THIS_SUB_ASSIGN_R64:
        VM_DEREFERENCE(vm.regs.frame.refs[-1],SLAGCODE_OPERAND_I32(),SlagReal64) -= SLAG_POP_REAL();
        continue;

      case SLAGOP_THIS_MUL_ASSIGN_R64:
        VM_DEREFERENCE(vm.regs.frame.refs[-1],SLAGCODE_OPERAND_I32(),SlagReal64) *= SLAG_POP_REAL();
        continue;

      case SLAGOP_THIS_INCREMENT_R64:
        ++VM_DEREFERENCE(vm.regs.frame.refs[-1],SLAGCODE_OPERAND_I32(),SlagReal64);
        continue;

      case SLAGOP_THIS_DECREMENT_R64:
        --VM_DEREFERENCE(vm.regs.frame.refs[-1],SLAGCODE_OPERAND_I32(),SlagReal64);
        continue;

      case SLAGOP_LOCAL_ADD_ASSIGN_INTEGER:
        vm.regs.frame.data[SLAGCODE_OPERAND_I32()] += SLAG_POP_INTEGER();
        continue;

      case SLAGOP_LOCAL_SUB_ASSIGN_INTEGER:
        vm.regs.frame.data[SLAGCODE_OPERAND_I32()] -= SLAG_POP_INTEGER();
        continue;

      case SLAGOP_LOCAL_MUL_ASSIGN_INTEGER:
        vm.regs.frame.data[SLAGCODE_OPERAND_I32()] *= SLAG_POP_INTEGER();
        continue;

      case SLAGOP_LOCAL_INCREMENT_INTEGER:
        ++vm.regs.frame.data[SLAGCODE_OPERAND_I32()];
        continue;

      case SLAGOP_LOCAL_DECREMENT_INTEGER:
        --vm.regs.frame.data[SLAGCODE_OPERAND_I32()];
        continue;

      case SLAGOP_LOCAL_ADD_ASSIGN_REAL:
        ((SlagReal64*)vm.regs.frame.data)[SLAGCODE_OPERAND_I32()] += SLAG_POP_REAL();
        continue;

      case SLAGOP_LOCAL_SUB_ASSIGN_REAL:
        ((SlagReal64*)vm.regs.frame.data)[SLAGCODE_OPERAND_I32()] -= SLAG_POP_REAL();
        continue;

      case SLAGOP_LOCAL_MUL_ASSIGN_REAL:
        ((SlagReal64*)vm.regs.frame.data)[SLAGCODE_OPERAND_I32()] *= SLAG_POP_REAL();
        continue;

      case SLAGOP_LOCAL_INCREMENT_REAL:
        ++((SlagReal64*)vm.regs.frame.data)[SLAGCODE_OPERAND_I32()];
        continue;

      case SLAGOP_LOCAL_DECREMENT_REAL:
        --((SlagReal64*)vm.regs.frame.data)[SLAGCODE_OPERAND_I32()];
        continue;

      default:
        {
          char buffer[80];
          sprintf( buffer, "Unhandled opcode: %d", *(--vm.regs.ip) );
          slag_throw_fatal_error( buffer );
        }
    }

    printf( "ERROR\n" );

    execute_call:
      // Calls method referenced by 'm'.
      if (vm.regs.call_frame < vm.min_call_stack)
      {
        throw_stack_limit_error();
      }
      // Save prior call frame.
      (--vm.regs.call_frame)->called_method    = m;
      vm.regs.call_frame->prior_frame          = vm.regs.frame;
      vm.regs.call_frame->return_address       = vm.regs.ip;

      // Set up current call frame pointers to be just before parameter data.
      vm.regs.frame.refs  = vm.regs.stack.refs  + m->parameters->num_ref_params;
      vm.regs.frame.data = vm.regs.stack.data + m->parameters->num_data_params;

      if (vm.regs.stack.data < vm.min_data_stack || vm.regs.stack.refs < vm.min_ref_stack)
      {
        throw_stack_limit_error();
      }

      // Adjust stack pointers to make room for local data & clear that region.
      {
        int count = m->num_local_refs;
        if (count)
        {
          vm.regs.stack.refs -= count;

          // Need to clear this in case a GC happens before all the locals
          // are initialized.
          memset( vm.regs.stack.refs, 0, count*sizeof(SlagObject*) );
        }

        vm.regs.stack.data -= m->local_data_size;
      }
      vm.regs.ip = m->bytecode;
      continue;
  }
}

static SlagCatchInfo* find_catch( SlagObject* err, int ip_offset )
{
  // Returns a compatible catch in the current method or NULL if not found.
  SlagMethodInfo* m = vm.regs.call_frame->called_method;
  for (int i=0; i<m->catches.count; ++i)
  {
    SlagCatchInfo* cur_catch = m->catches[i];
    if (ip_offset >= cur_catch->begin_offset && ip_offset <= cur_catch->end_offset)
    {
      if (err->type->instance_of(cur_catch->type_caught)) return cur_catch;
    }
  }
  return NULL;
}


static SlagCatchInfo* unwind_stack( SlagObject* err )
{
  // Unwind call frames until a suitable 'catch' is found.
  SlagCatchInfo* catch_info = NULL;
  while (vm.regs.call_frame < vm.call_stack_limit)
  {
    int ip_offset = ((SlagOpcode*)(void*)vm.regs.ip) - vm.code.data;
    catch_info = find_catch(err,ip_offset);
    if (catch_info) break;
    vm.regs.stack = vm.regs.frame;
    vm.regs.frame = vm.regs.call_frame->prior_frame;
    vm.regs.ip = (vm.regs.call_frame++)->return_address;
  }
  return catch_info;
}

static void activate_catch( SlagCatchInfo* catch_info, SlagObject* err )
{
  if ( !catch_info )
  {
    // Do a to_String on the error, save it as the global error message,
    // and abort the program.
    SlagMethodInfo* m_to_string = err->type->must_find_method("to_String()");
    SLAG_PUSH_REF( err );
    vm.call( m_to_string );
    SlagString* mesg = (SlagString*) SLAG_POP_REF();
    slag_error_message = mesg->to_new_ascii();
#if SLAG_USE_LONGJMP
    longjmp( slag_fatal_jumppoint, 1 );
#else
    throw 1;
#endif
  }

  vm.regs.ip = vm.code.data + catch_info->handler;
  SLAG_PUSH_REF( err );
}

void SlagVM::throw_exception_on_stack()
{
  // Pop off exception object
  SlagObject* err = SLAG_POP_REF();

  // Handle null exception reference
  if ( !err )
  {
    slag_throw_fatal_error( "Thrown exception is null." );
  }

  activate_catch( unwind_stack(err), err );
}

void SlagVM::throw_exception( SlagTypeInfo* type, const char* mesg )
{
  exception_ip = regs.ip;

  SlagObject* err = type->create();
  SLAG_PUSH_REF( err ); // result on stack
  SLAG_PUSH_REF( err ); // for init() / init(String)
  SLAG_PUSH_REF( err ); // for init_object()

  call( type->method_init_object );

  if (mesg)
  {
    SLAG_PUSH_REF( (SlagObject*) SlagString::create(mesg) );
    call( type->must_find_method("init(String)") );
  }
  else
  {
    call( type->must_find_method("init()") );
  }

  throw_exception_on_stack();
}

void SlagVM::throw_stack_limit_error()
{
  // Stack limit errors are a little different since we need to
  // save the history, create the error object, unwind the stack, 
  // and init the error object.
  exception_ip = regs.ip;

  ArrayList<SlagInt64> history;
  get_call_history(history);

  SlagTypeInfo* type = vm.type_stack_limit_error;
  SlagObject* err = type->create();

  SlagCatchInfo* catch_info = unwind_stack(err);

  // initialize the object now that we've got some breathing room
  SLAG_PUSH_REF( err );
  SlagArray* array = vm.type_array_of_int64->create( history.count );
  memcpy( array->data, history.data, history.count * 8 );
  SLAG_PUSH_REF( array );
  call( type->must_find_method("init(Array<<Int64>>)") );

  activate_catch( catch_info, err );
}

//=============================================================================
//  Loader
//=============================================================================
#define SLAGCMD_SET_FILENAME_AND_LINE      0
#define SLAGCMD_SET_LINE                   1
#define SLAGCMD_INC_LINE                   2
#define SLAGCMD_STRUCTURE_ID               3
#define SLAGCMD_RETURN_VALUE               4
#define SLAGCMD_RETURN_NIL                 5
#define SLAGCMD_BLOCK                      6
#define SLAGCMD_TRYCATCH                   7
#define SLAGCMD_THROW                      8
#define SLAGCMD_IF                         9
#define SLAGCMD_CONTINGENT                10
#define SLAGCMD_NECESSARY                 11
#define SLAGCMD_SUFFICIENT                12
#define SLAGCMD_WHILE                     13
#define SLAGCMD_LOOP                      14
#define SLAGCMD_ESCAPE                    15
#define SLAGCMD_NEXT_ITERATION            16
#define SLAGCMD_BREAKPOINT                17
#define SLAGCMD_LITERAL_STRING            18
#define SLAGCMD_LITERAL_INT64             19
#define SLAGCMD_LITERAL_INT32             20
#define SLAGCMD_LITERAL_CHAR              21
#define SLAGCMD_LITERAL_BYTE              22
#define SLAGCMD_LITERAL_REAL64            23
#define SLAGCMD_LITERAL_REAL32            24
#define SLAGCMD_LITERAL_LOGICAL_TRUE      25
#define SLAGCMD_LITERAL_LOGICAL_FALSE     26
#define SLAGCMD_LITERAL_LOGICAL_VOID      27
#define SLAGCMD_LITERAL_NULL              28
#define SLAGCMD_THIS_REFERENCE            29
#define SLAGCMD_SINGLETON_REFERENCE       30
#define SLAGCMD_SINGLETON_WRITE           31
#define SLAGCMD_PROPERTY_READ             32
#define SLAGCMD_COMPOUND_PROPERTY_READ    33
#define SLAGCMD_LOCAL_VAR_READ            34
#define SLAGCMD_PROPERTY_WRITE            35
#define SLAGCMD_LOCAL_VAR_WRITE           36
#define SLAGCMD_STATIC_CALL               37
#define SLAGCMD_DYNAMIC_CALL              38
#define SLAGCMD_NEW_OBJECT                39
#define SLAGCMD_NEW_COMPOUND              40
#define SLAGCMD_ARRAY_CREATE              41
#define SLAGCMD_ARRAY_GET                 42
#define SLAGCMD_ARRAY_SET                 43
#define SLAGCMD_NARROWING_CAST            44
#define SLAGCMD_WIDENING_CAST             45
#define SLAGCMD_CAST_TO_INT64             46
#define SLAGCMD_CAST_TO_INT32             47
#define SLAGCMD_CAST_TO_CHAR              48
#define SLAGCMD_CAST_TO_BYTE              49
#define SLAGCMD_CAST_TO_REAL64            50
#define SLAGCMD_CAST_TO_REAL32            51
#define SLAGCMD_CAST_TO_LOGICAL           52
#define SLAGCMD_AS                        53
#define SLAGCMD_COERCE_AS                 54
#define SLAGCMD_CMP_INSTANCE_OF           55
#define SLAGCMD_CMP_IS                    56
#define SLAGCMD_CMP_IS_NOT                57
#define SLAGCMD_CMP_IS_NULL               58
#define SLAGCMD_CMP_IS_NOT_NULL           59
#define SLAGCMD_CMP_EQ                    60
#define SLAGCMD_CMP_NE                    61
#define SLAGCMD_CMP_LE                    62
#define SLAGCMD_CMP_LT                    63
#define SLAGCMD_CMP_GT                    64
#define SLAGCMD_CMP_GE                    65
#define SLAGCMD_NOT                       66
#define SLAGCMD_NEGATE                    67
#define SLAGCMD_ADD                       68
#define SLAGCMD_SUB                       69
#define SLAGCMD_MUL                       70
#define SLAGCMD_DIV                       71
#define SLAGCMD_MOD                       72
#define SLAGCMD_EXP                       73
#define SLAGCMD_AND                       74
#define SLAGCMD_OR                        75
#define SLAGCMD_XOR                       76
#define SLAGCMD_LAZY_AND                  77
#define SLAGCMD_LAZY_OR                   78
#define SLAGCMD_LEFT_SHIFTED              79
#define SLAGCMD_RIGHT_SHIFTED             80
#define SLAGCMD_RIGHT_XSHIFTED            81
#define SLAGCMD_THIS_ADD_AND_ASSIGN       82
#define SLAGCMD_THIS_SUB_AND_ASSIGN       83
#define SLAGCMD_THIS_MUL_AND_ASSIGN       84
#define SLAGCMD_THIS_DIV_AND_ASSIGN       85
#define SLAGCMD_THIS_MOD_AND_ASSIGN       86
#define SLAGCMD_LOCAL_VAR_ADD_AND_ASSIGN  87
#define SLAGCMD_LOCAL_VAR_SUB_AND_ASSIGN  88
#define SLAGCMD_LOCAL_VAR_MUL_AND_ASSIGN  89
#define SLAGCMD_LOCAL_VAR_DIV_AND_ASSIGN  90
#define SLAGCMD_LOCAL_VAR_MOD_AND_ASSIGN  91
#define SLAGCMD_THIS_INCREMENT            92
#define SLAGCMD_THIS_DECREMENT            93
#define SLAGCMD_LOCAL_INCREMENT           94
#define SLAGCMD_LOCAL_DECREMENT           95
#define SLAGCMD_ARRAY_DUPLICATE           96

void SlagVM_unhandled_native_method()
{
  SlagMethodInfo* m = vm.regs.call_frame->called_method;
  slag_throw_fatal_error( "Unhandled native call to ", m->type_context->name, 
      "::", m->signature, "." );
}


//=============================================================================
//  SlagReader
//=============================================================================
SlagReader::~SlagReader()
{
  if (free_data && data) delete data;
  data = 0;
  remaining = 0;
  pos = 0;
}

void SlagReader::init( const char* original_filename )
{
  int len = strlen(original_filename);
  if (len > 250) slag_throw_fatal_error( "Filename too long." );

  char filename[256];
  strcpy( filename, original_filename );

  FILE* fp = fopen( filename, "rb" );
  if ( !fp )
  {
    if (len > 5)
    {
      if (0 == strcmp( filename+(len-5), ".slag" ) )
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
      slag_throw_fatal_error( "Error opening file \"", filename, "\"." );
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

void SlagReader::init( const unsigned char* data, int size )
{
  this->data = data;
  pos = 0;
  remaining = size;
  free_data = false;
  total_size = remaining;
}

int SlagReader::read()
{
  if (pos == remaining) slag_throw_fatal_error( "Unexpected EOF" );
  return data[pos++];
}

int SlagReader::read16()
{
  if (pos+2 > remaining) slag_throw_fatal_error( "Unexpected EOF" );
  int result = data[pos++];
  result = (result << 8) | data[pos++];
  return result;
}

int SlagReader::read32()
{
  if (pos+4 > remaining) slag_throw_fatal_error( "Unexpected EOF" );
  int result = data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  return result;
}

SlagInt64 SlagReader::read64()
{
  if (pos+8 > remaining) slag_throw_fatal_error( "Unexpected EOF" );
  SlagInt64 result = data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  result = (result << 8) | data[pos++];
  return result;
}

int SlagReader::readX()
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

int SlagReader::read_utf8()
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

char* SlagReader::read_new_ascii()
{
  int count = readX();
  char* result = new char[count+1];
  for (int i=0; i<count; ++i) result[i] = read();
  result[count] = 0;
  return result;
}

//=============================================================================
//  SlagLoader
//=============================================================================
void SlagLoader::load( const char* filename )
{
  reader.init( filename );
  load();
}

void SlagLoader::load( const char* data, int count )
{
  reader.init( (const unsigned char*) data, count );
  load();
}

void SlagLoader::load()
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

void SlagLoader::load_version()
{
  must_consume_header( "ETC" );
  reader.read32();  // discard version
}

void SlagLoader::load_filenames()
{
  must_consume_header( "FILENAMES" );
  int n = reader.readX();
  vm.filenames.ensure_capacity(n);
  for (int i=0; i<n; ++i) vm.filenames.add( reader.read_new_ascii() );
}

void SlagLoader::load_identifiers()
{
  must_consume_header( "IDENTIFIERS" );
  int n = reader.readX();
  vm.identifiers.ensure_capacity(n);
  for (int i=0; i<n; ++i) vm.identifiers.add( reader.read_new_ascii() );
}

void SlagLoader::load_type_info()
{
  must_consume_header( "TYPEINFO" );
  int n = reader.readX();
  vm.types.ensure_capacity(n);
  for (int i=0; i<n; ++i)
  {
    vm.types.add( new SlagTypeInfo() );
  }

  for (int i=0; i<n; ++i)
  {
    SlagTypeInfo* type = vm.types[i];
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
  vm.main_class = load_type();

  vm.type_object  = vm.must_find_type( "Object" );
  vm.type_int64   = vm.must_find_type( "Int64" );
  vm.type_int32   = vm.must_find_type( "Int32" );
  vm.type_char    = vm.must_find_type( "Char" );
  vm.type_byte    = vm.must_find_type( "Byte" );
  vm.type_real64  = vm.must_find_type( "Real64" );
  vm.type_real32  = vm.must_find_type( "Real32" );
  vm.type_logical = vm.must_find_type( "Logical" );
  vm.type_null    = vm.must_find_type( "null" );
  vm.type_string  = vm.must_find_type( "String" );
  vm.type_system  = vm.must_find_type( "System" );
  vm.type_native_data           = vm.must_find_type( "NativeData" );
  vm.type_weak_reference        = vm.must_find_type( "WeakReference" );
  vm.type_array_of_char         = vm.must_find_type( "Array<<Char>>" );
  vm.type_array_of_int64        = vm.must_find_type( "Array<<Int64>>" );
  vm.type_requires_cleanup      = vm.must_find_type( "RequiresCleanup" );
  vm.type_missing_return_error  = vm.must_find_type( "MissingReturnError" );
  vm.type_type_cast_error       = vm.must_find_type( "TypeCastError" );
  vm.type_out_of_bounds_error   = vm.must_find_type( "OutOfBoundsError" );
  vm.type_divide_by_zero_error  = vm.must_find_type( "DivideByZeroError" );
  vm.type_null_reference_error  = vm.must_find_type( "NullReferenceError" );
  vm.type_stack_limit_error     = vm.must_find_type( "CallStackLimitReached" );
  vm.type_file_error            = vm.must_find_type( "FileError" );
  vm.type_file_not_found_error  = vm.must_find_type( "FileNotFoundError" );
  vm.type_no_next_value_error   = vm.must_find_type( "NoNextValueError" );
  vm.type_socket_error          = vm.must_find_type( "SocketError" );
  vm.type_invalid_operand_error = vm.must_find_type( "InvalidOperandError" );

}

void SlagLoader::load_parameter_info()
{
  must_consume_header( "PARAMETERINFO" );
  int n = reader.readX();
  vm.parameter_table.ensure_capacity(n);

  for (int i=0; i<n; ++i)
  {
    int num_params = reader.readX();
    SlagParameterList* list = new SlagParameterList(num_params);
    vm.parameter_table.add( list );

    for (int i=0; i<num_params; ++i)
    {
      list->parameters.add( new SlagLocalVarInfo(load_type()) );
    }
  }
}

void SlagLoader::load_method_info()
{
  must_consume_header( "METHODINFO" );
  int n = reader.readX();
  vm.methods.ensure_capacity(n);

  for (int i=0; i<n; ++i)
  {
    SlagMethodInfo* m = new SlagMethodInfo();
    vm.methods.add(m);

    m->name = load_id();
    m->index = i;
    m->qualifiers = reader.readX();
    m->type_context = load_type();
    m->parameters = vm.parameter_table[ reader.readX() ];

    int index = reader.readX();
    m->return_type = (index==-1) ? NULL : vm.types[index];
    m->catches.ensure_capacity( reader.readX() );

    int num_locals = reader.readX();
    m->local_vars.ensure_capacity( num_locals );
    for (int v=0; v<num_locals; ++v)
    {
      m->local_vars.add( new SlagLocalVarInfo(load_type()) );
    }

    if (m->qualifiers & SLAG_QUALIFIER_NATIVE)
    {
      m->native_handler = (SlagNativeFn) SlagVM_unhandled_native_method;
    }
  }
}

void SlagLoader::load_type_defs()
{
  must_consume_header( "TYPEDEFS" );

  for (int t=0; t<vm.types.count; ++t)
  {
    SlagTypeInfo* type = vm.types[t];

    int n = reader.readX();
    type->properties.ensure_capacity( n );

    int num_reference_properties = 0;
    for (int i=0; i<n; ++i)
    {
      SlagPropertyInfo* v = new SlagPropertyInfo( load_type() );
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

  vm.prep_types_and_methods();

  // Set array element sizes
  for (int i=0; i<vm.types.count; ++i)
  {
    SlagTypeInfo* type = vm.types[i];
    SlagTypeInfo* element_type = (SlagTypeInfo*) type->element_type;
    if (element_type)
    {
      type->qualifiers |= SLAG_QUALIFIER_ARRAY;
      if (element_type->is_reference())
      {
        type->element_size = sizeof(void*);
        type->qualifiers |= SLAG_QUALIFIER_REFERENCE_ARRAY;
      }
      else if (element_type == vm.type_int64)
      {
        type->element_size = 8;
      }
      else if (element_type == vm.type_int32)
      {
        type->element_size = 4;
      }
      else if (element_type == vm.type_char)
      {
        type->element_size = 2;
      }
      else if (element_type == vm.type_byte)
      {
        type->element_size = 1;
      }
      else if (element_type == vm.type_real64)
      {
        type->element_size = 8;
      }
      else if (element_type == vm.type_real32)
      {
        type->element_size = 4;
      }
      else if (element_type == vm.type_logical)
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

void SlagLoader::load_string_table()
{
  must_consume_header( "STRINGTABLE" );

  int n = reader.readX();
  vm.strings.ensure_capacity(n);

  for (int i=0; i<n; ++i)
  {
    int count = reader.readX();
    SlagString* string = (SlagString*) 
      (mm.create_object( vm.type_string, (sizeof(SlagString)-2) + count*2 ));
    ++string->reference_count;
    string->count = count;

    SlagChar*   data = (SlagChar*) string->characters;
    ++count;
    --data;
    while (--count) *(++data) = reader.read_utf8();

    string->set_hash_code();

    vm.strings.add( string );
  }
}

void SlagLoader::load_method_defs()
{
  must_consume_header( "METHODDEFS" );

  vm.code.ensure_capacity( reader.total_size / 8 ); // seems to be a decent metric
  vm.line_table.ensure_capacity( reader.total_size / 16 );

  for (int i=0; i<vm.methods.count; ++i)
  {
    load_method_body( vm.methods[i] );
  }

  // Now that all code has been loaded (and code buffer has finished sizing),
  // determine actual code addresses from offsets.
  for (int i=0; i<vm.methods.count; ++i)
  {
    SlagMethodInfo* m = vm.methods[i];
    m->bytecode = vm.code.data + m->bytecode_offset;
  }

  for (int i=0; i<vm.address_offsets.count; ++i)
  {
    vm.address_table.add( vm.code.data + vm.address_offsets[i] );
  }
}

void SlagLoader::load_method_body( SlagMethodInfo* m )
{
  this_method = m;

  int code_count = vm.code.count;
  this_method->bytecode_offset = code_count;

  this_method->source_pos_offset = vm.line_table.count;
  next_structure_id = -1;
  next_auto_id = -2;

  resolved_labels.clear();

  load_statement_list();

  if (unresolved_labels.count)
  {
    slag_throw_fatal_error("Unresolved labels in ",m->type_context->name,"::",m->signature,".");
  }

  if (this_method->native_handler)
  {
    // Embed the native handler in a minimal body - this makes it easy to handle
    // stack frames in a consistent way.  Since dynamic method calls can call
    // a VM method *or* a native method, we do need to support the stack frame
    // system.

    // Methods can be flagged as native at load time - undo any method body
    // that's loaded so far.
    vm.code.count = code_count;

    write_op_i32( SLAGOP_NATIVE_CALL, this_method->index );

    if (this_method->return_type)
    {
      if (this_method->return_type->is_reference()) 
      {
        write_op( SLAGOP_RETURN_REF );
      }
      else
      {
        int size = this_method->return_type->stack_slots;
        if (size == 1) write_op( SLAGOP_RETURN_8 );
        else           write_op_i32( SLAGOP_RETURN_X, (SlagInt32) size );
      }
    }
    else
    {
      write_op( SLAGOP_RETURN_NIL );
    }
  }
  else
  {
    // Supply a catch-all return in case the user left off the return.

    if (this_method->return_type) 
    {
      write_op( SLAGOP_MISSING_RETURN_ERROR );
    }
    else
    {
      write_op( SLAGOP_RETURN_NIL );
    }
  }
  this_method->bytecode_limit = vm.code.count;
}

SlagTypeInfo* SlagLoader::load_type()
{
  int index = reader.readX();
  if (index == -1) return NULL;

  if (index < 0 || index >= vm.types.count)
  {
    slag_throw_fatal_error( "Type index out of bounds in loader." );
  }

  return vm.types[index];
}

SlagMethodInfo* SlagLoader::load_method()
{
  int index = reader.readX();
  if (index == -1) return NULL;

  if (index < 0 || index >= vm.methods.count)
  {
    slag_throw_fatal_error( "Method index out of bounds in loader." );
  }

  return vm.methods[index];
}

char* SlagLoader::load_id()
{
  int index = reader.readX();
  if (index < 0 || index >= vm.identifiers.count)
  {
    slag_throw_fatal_error( "ID index out of bounds in loader." );
  }

  return vm.identifiers[index];
}

SlagLocalVarInfo* SlagLoader::load_local()
{
  int index = reader.readX();
  if (index < this_method->parameters->count()) return this_method->parameters->get(index);
  else return this_method->local_vars[index - this_method->parameters->count()];
}

SlagPropertyInfo* SlagLoader::load_this_property()
{
  return this_method->type_context->properties[ reader.readX() ];
}


void SlagLoader::must_consume_header( const char* header_id )
{
  int count = reader.read();
  for (int i=0; i<count; ++i)
  {
    if (reader.read() != header_id[i]) 
    {
      slag_throw_fatal_error( "Error reading header '", header_id, "'." );
    }
  }
  if (header_id[count] != 0)
  {
    slag_throw_fatal_error( "Error reading header '", header_id, "'." );
  }
}

void SlagLoader::write_op( int op )
{
  if (op_history.count >= 4) op_history.remove(0);
  op_history.add( SlagOpInfo(op,vm.code.count) );

  vm.code.add(op);
}

void SlagLoader::write_i32( SlagInt32 value )
{
  vm.code.add( value );
}

void SlagLoader::write_op_i32( int op, SlagInt32 value )
{
  write_op(op);
  vm.code.add( value );
}

void SlagLoader::write_op_i64( int op, SlagInt64 value )
{
  write_op(op);
  vm.code.add( vm.literal_table.count );
  vm.literal_table.add( value );
}

int SlagLoader::history( int num_back )
{
  if (op_history.count < num_back) return 0;
  return op_history[ op_history.count - num_back ].op;
}

void SlagLoader::undo_op( int count )
{
  while (--count) op_history.remove_last();

  vm.code.count = op_history.last().code_offset;
  op_history.remove_last();
}

void SlagLoader::load_statement_list()
{
  int n = reader.readX();
  while (n--)
  {
    SlagTypeInfo* type = load_statement();
    if (type)
    {
      // Get rid of bytes left on stack.
      if (type->is_reference()) write_op( SLAGOP_POP_REF );
      else 
      {
        int size = type->stack_slots;
        if (size == 1) write_op( SLAGOP_POP_8 );
        else           write_op_i32( SLAGOP_POP_X, (SlagInt32) size );
      }
    }
  }
}

SlagTypeInfo* SlagLoader::load_statement()
{
  for (;;)
  {
    int op = reader.readX();
    switch (op)
    {
      case SLAGCMD_SET_FILENAME_AND_LINE:
        // filename index <= 0
        vm.line_table.add( SlagSourcePos( vm.code.count, -reader.readX() ) );

        cur_line = reader.readX();
        vm.line_table.add( SlagSourcePos( vm.code.count, cur_line ) );
        continue;

      case SLAGCMD_SET_LINE:
        cur_line = reader.readX();
        vm.line_table.add( SlagSourcePos( vm.code.count, cur_line ) );
        continue;

      case SLAGCMD_INC_LINE:
        vm.line_table.add( SlagSourcePos( vm.code.count, ++cur_line ) );
        continue;

      case SLAGCMD_STRUCTURE_ID:
        next_structure_id = reader.readX();
        continue;

      case SLAGCMD_RETURN_VALUE:
        {
          SlagTypeInfo* type = load_expression();
          if (type->is_reference()) 
          {
            write_op( SLAGOP_RETURN_REF );
          }
          else
          {
            int size = type->stack_slots;
            if (size == 1) write_op( SLAGOP_RETURN_8 );
            else           write_op_i32( SLAGOP_RETURN_X, (SlagInt32) size );
          }
        }
        break;

      case SLAGCMD_RETURN_NIL:
        write_op( SLAGOP_RETURN_NIL );
        break;

      case SLAGCMD_BLOCK:
        {
          int id = get_next_structure_id();
          load_statement_list();
          define_label( "end", id, 0 );
          close_label_id( id );
        }
        break;

      case SLAGCMD_TRYCATCH:
        {
          int id = get_next_structure_id();
          int try_block_begin = vm.code.count;

          load_statement_list(); 
          int try_block_end = vm.code.count;
          
          int num_catches = reader.readX();
          if (num_catches) 
          {
            write_op( SLAGOP_JUMP );
            write_label_address( "end", id, 0 );

            while (num_catches--)
            {
              int catch_var = reader.readX() - this_method->parameters->count();

              this_method->catches.add(
                  new SlagCatchInfo(
                    this_method->local_vars[catch_var]->type,
                    try_block_begin, try_block_end,
                    vm.code.count
                  )
                );

              // Insert code to copy the exception reference into the catch's
              // designated local variable.
              write_op_i32( 
                  SLAGOP_WRITE_LOCAL_REF, 
                  (SlagInt32) this_method->local_vars[catch_var]->offset 
                  );

              load_statement_list();

              write_op( SLAGOP_JUMP );
              write_label_address( "end", id, 0 );
            }
          }

          define_label( "end", id, 0 );
          close_label_id( id );
        }
        break;

      case SLAGCMD_THROW:
        load_ref_expression();
        write_op( SLAGOP_THROW );
        break;

      case SLAGCMD_IF:
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
              write_op( SLAGOP_JUMP );
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
                write_op( SLAGOP_JUMP );
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

      case SLAGCMD_CONTINGENT:
        {
          int id = get_next_structure_id();
          load_statement_list();

          define_label( "satisfied", id, 0 );
          load_statement_list();
          write_op( SLAGOP_JUMP );
          write_label_address( "end", id, 0 );

          define_label( "unsatisfied", id, 0 );
          load_statement_list();

          define_label( "end", id, 0 );
          close_label_id( id );
        }
        break;

      case SLAGCMD_NECESSARY:
        {
          int id = reader.readX();
          load_logical_expression();
          write_jump_if_false( "unsatisfied", id, 0 );
        }
        break;

      case SLAGCMD_SUFFICIENT:
        {
          int id = reader.readX();
          load_logical_expression();
          write_jump_if_true( "satisfied", id, 0 );
        }
        break;

      case SLAGCMD_WHILE:
        {
          int id = get_next_structure_id();
          write_op( SLAGOP_JUMP );
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

      case SLAGCMD_LOOP:
        {
          int id = get_next_structure_id();

          define_label( "loop", id, 0 );
          define_label( "termination-test", id, 0 );
          load_statement_list();

          write_op( SLAGOP_JUMP );
          write_label_address( "loop", id, 0 );

          define_label( "end", id, 0 );
          close_label_id( id );
        }
        break;

      case SLAGCMD_ESCAPE:
        {
          int id = reader.readX();
          write_op( SLAGOP_JUMP );
          write_label_address( "end", id, 0 );
        }
        break;

      case SLAGCMD_NEXT_ITERATION:
        {
          int id = reader.readX();
          write_op( SLAGOP_JUMP );
          write_label_address( "termination-test", id, 0 );
        }
        break;

      case SLAGCMD_BREAKPOINT:
        {
          int id = reader.readX();
          write_op_i32( SLAGOP_BREAKPOINT, (SlagInt32) id );
        }
        break;

      case SLAGCMD_LITERAL_STRING:
        {
          int index = reader.readX();
          write_op_i32( SLAGOP_LITERAL_STRING, index );
          return vm.type_string;
        }

      case SLAGCMD_LITERAL_INT64:
        {
          SlagInt64 value = reader.read64();
          switch (value)
          {
            case 1:  write_op( SLAGOP_LITERAL_INTEGER_1 ); break;
            case 0:  write_op( SLAGOP_LITERAL_INTEGER_0 ); break;
            case -1: write_op( SLAGOP_LITERAL_INTEGER_NEG1 ); break;
            default:
              write_op_i64( SLAGOP_LITERAL_8, value );
              break;
          }
          return vm.type_int64;
        }

      case SLAGCMD_LITERAL_INT32:
        {
          SlagInt32 value = reader.readX();
          switch (value)
          {
            case 1:  write_op( SLAGOP_LITERAL_INTEGER_1 ); break;
            case 0:  write_op( SLAGOP_LITERAL_INTEGER_0 ); break;
            case -1: write_op( SLAGOP_LITERAL_INTEGER_NEG1 ); break;
            default:
              write_op_i32( SLAGOP_LITERAL_4, value );
              break;
          }
          return vm.type_int32;
        }

      case SLAGCMD_LITERAL_CHAR:
        {
          write_op_i32( SLAGOP_LITERAL_4, (SlagInt32) reader.readX() );
        }
        return vm.type_char;

      case SLAGCMD_LITERAL_BYTE:
        {
          write_op_i32( SLAGOP_LITERAL_4, (SlagInt32) reader.readX() );
        }
        return vm.type_byte;

      case SLAGCMD_LITERAL_REAL64:
        {
          SlagInt64  value = reader.read64();
          SlagReal64 real_value = *((SlagReal64*) &value);
          if (real_value == 1.0)       write_op( SLAGOP_LITERAL_REAL_1 );
          else if (real_value == 0.0)  write_op( SLAGOP_LITERAL_REAL_0 );
          else if (real_value == -1.0) write_op( SLAGOP_LITERAL_REAL_NEG1 );
          else
          {
            write_op_i64( SLAGOP_LITERAL_8, value );
          }
        }
        return vm.type_real64;

      case SLAGCMD_LITERAL_REAL32:
        {
          write_op_i32( SLAGOP_LITERAL_REAL32, (SlagInt32) reader.readX() );
        }
        return vm.type_real32;

      case SLAGCMD_LITERAL_LOGICAL_TRUE:
        write_op( SLAGOP_LITERAL_INTEGER_1 );
        return vm.type_logical;

      case SLAGCMD_LITERAL_LOGICAL_FALSE:
        write_op( SLAGOP_LITERAL_INTEGER_0 );
        return vm.type_logical;

      case SLAGCMD_LITERAL_LOGICAL_VOID:
        write_op( SLAGOP_LITERAL_INTEGER_NEG1 );
        return vm.type_logical;

      case SLAGCMD_LITERAL_NULL:
        write_op( SLAGOP_LITERAL_NULL );
        return vm.type_null;

      case SLAGCMD_THIS_REFERENCE:
        write_op( SLAGOP_READ_THIS_REF );
        return this_method->type_context;

      case SLAGCMD_SINGLETON_REFERENCE:
        {
          SlagTypeInfo* type = load_type();
          write_op_i32( SLAGOP_READ_SINGLETON_REF, type->singleton_index );
          return type;
        }
        break;

      case SLAGCMD_SINGLETON_WRITE:
        {
          SlagTypeInfo* type = load_type();
          load_ref_expression();
          write_op_i32( SLAGOP_WRITE_SINGLETON_REF, type->singleton_index );
        }
        break;

      case SLAGCMD_PROPERTY_READ:
        {
          SlagTypeInfo* context_type = load_ref_expression();
          int index = reader.readX();
          SlagInt32 offset = context_type->properties[index]->offset;
          SlagTypeInfo* var_type = context_type->properties[index]->type;

          int last_op = history(1);
          if (var_type->is_reference())
          {
            if (last_op == SLAGOP_READ_THIS_REF)
            {
              undo_op();
              write_op_i32( SLAGOP_READ_THIS_PROPERTY_REF, offset );
            }
            else
            {
              write_op_i32( SLAGOP_READ_PROPERTY_REF, offset );
            }
          }
          else 
          {
            SlagInt32 size = var_type->object_size;
            switch (size)
            {
              case 1:  
                write_op_i32(SLAGOP_READ_PROPERTY_1U,offset);
                break;

              case 2:
                write_op_i32(SLAGOP_READ_PROPERTY_2U,offset);
                break;
              case 4:  
                if (last_op == SLAGOP_READ_THIS_REF)
                {
                  undo_op();
                  if (var_type == vm.type_real32) write_op_i32( SLAGOP_READ_THIS_PROPERTY_REAL32, offset );
                  else                             write_op_i32( SLAGOP_READ_THIS_PROPERTY_4, offset );
                }
                else
                {
                  if (var_type == vm.type_real32) write_op_i32( SLAGOP_READ_PROPERTY_REAL32, offset );
                  else                             write_op_i32( SLAGOP_READ_PROPERTY_4, offset );
                }
                break;
              case 8:  
                if (last_op == SLAGOP_READ_THIS_REF)
                {
                  undo_op();
                  write_op_i32( SLAGOP_READ_THIS_PROPERTY_8, offset );
                }
                else
                {
                  write_op_i32( SLAGOP_READ_PROPERTY_8, offset );
                }
                break;
              default:
                if (last_op == SLAGOP_READ_THIS_REF)
                {
                  undo_op();
                  write_op_i32( SLAGOP_READ_THIS_PROPERTY_X, offset );
                  write_i32( size );
                }
                else
                {
                  write_op_i32( SLAGOP_READ_PROPERTY_X, offset );
                  write_i32( size );
                }
                break;
            }
          }
          return var_type;
        }
        break;

      case SLAGCMD_COMPOUND_PROPERTY_READ:
        {
          SlagTypeInfo* context_type = load_compound_expression();
          int index = reader.readX();
          SlagInt32 offset = context_type->properties[index]->offset;
          SlagTypeInfo* var_type = context_type->properties[index]->type;
          SlagInt32 slots = var_type->stack_slots;

          // Don't need to do anything if the compound element is the same size
          // as the compound itself.
          if (slots == context_type->stack_slots) return var_type;

          int last_op = history(0);
          switch (slots)
          {
            case 1:  
              if (last_op == SLAGOP_READ_THIS_PROPERTY_X)
              {
                offset += vm.code[vm.code.count-2];
                undo_op();
                write_op_i32( SLAGOP_READ_THIS_PROPERTY_8, offset );
                return var_type;
              }
              else if (last_op == SLAGOP_READ_PROPERTY_X)
              {
                offset += vm.code[vm.code.count-2];
                undo_op();
                write_op_i32( SLAGOP_READ_PROPERTY_8, offset );
                return var_type;
              }
              else if (last_op == SLAGOP_READ_LOCAL_X)
              {
                offset += vm.code[vm.code.count-2];
                undo_op();
                write_op_i32( SLAGOP_READ_LOCAL_8, offset );
                return var_type;
              }
              else
              {
                write_op( SLAGOP_READ_COMPOUND_8 ); 
              }
              break;

            default:
              if (last_op == SLAGOP_READ_THIS_PROPERTY_X || last_op == SLAGOP_READ_PROPERTY_X
                  || last_op == SLAGOP_READ_LOCAL_X)
              {
                vm.code[vm.code.count-2] += offset;
                vm.code[vm.code.count-1] = slots;
                return var_type;
              }
              else
              {
                write_op( SLAGOP_READ_COMPOUND_X );
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

      case SLAGCMD_LOCAL_VAR_READ:
        {
          SlagInt32 offset;
          SlagTypeInfo* var_type;
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
            write_op_i32( SLAGOP_READ_LOCAL_REF, offset );
          }
          else 
          {
            SlagInt32 size = var_type->stack_slots;
            switch (size)
            {
              case 1:  write_op_i32( SLAGOP_READ_LOCAL_8, offset ); break;
              default:
                write_op_i32( SLAGOP_READ_LOCAL_X, offset );
                write_i32( size );
                break;
            }
          }
          return var_type;
        }
        break;

      case SLAGCMD_PROPERTY_WRITE:
        {
          SlagTypeInfo* context_type = load_ref_expression();

          int index = reader.readX();
          bool read_this = false;
          SlagTypeInfo* var_type = context_type->properties[index]->type;
          SlagInt32 offset = context_type->properties[index]->offset;
          SlagInt32 size = var_type->object_size;

          if ( (history(0) == SLAGOP_READ_THIS_REF)
              && (var_type->is_reference() || size >= 4) )
          {
            read_this = true;
            undo_op();
          }

          load_expression();

          if (read_this)
          {
            int write_op;
            if (var_type->is_reference())         write_op = SLAGOP_WRITE_THIS_PROPERTY_REF;
            else if (var_type == vm.type_real32) write_op = SLAGOP_WRITE_THIS_PROPERTY_REAL32;
            else if (size == 4)                   write_op = SLAGOP_WRITE_THIS_PROPERTY_4;
            else if (size == 8)                   write_op = SLAGOP_WRITE_THIS_PROPERTY_8;
            else                                  write_op = SLAGOP_WRITE_THIS_PROPERTY_X;
            write_op_i32( write_op, offset );
            if (write_op == SLAGOP_WRITE_THIS_PROPERTY_X) 
            {
              write_i32( size );
            }
          }
          else
          {
            if (var_type->is_reference())
            {
              write_op_i32( SLAGOP_WRITE_PROPERTY_REF, offset );
            }
            else if (var_type == vm.type_real32)
            {
              write_op_i32( SLAGOP_WRITE_PROPERTY_REAL32, offset ); break;
            }
            else
            {
              switch (size)
              {
                case 1: write_op_i32( SLAGOP_WRITE_PROPERTY_1, offset ); break;
                case 2: write_op_i32( SLAGOP_WRITE_PROPERTY_2, offset ); break;
                case 4: write_op_i32( SLAGOP_WRITE_PROPERTY_4, offset ); break;
                case 8: write_op_i32( SLAGOP_WRITE_PROPERTY_8, offset ); break;
                default:
                  write_op_i32( SLAGOP_WRITE_PROPERTY_X, offset );
                  write_i32( size );
                  break;
              }
            }
          }
        }
        break;

      case SLAGCMD_LOCAL_VAR_WRITE:
        {
          SlagInt32 offset;
          SlagTypeInfo* var_type;
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
            write_op_i32( SLAGOP_WRITE_LOCAL_REF, offset );
          }
          else
          {
            SlagInt32 slots = var_type->stack_slots;
            switch (slots)
            {
              case 1:  write_op_i32( SLAGOP_WRITE_LOCAL_8, offset ); break;
              default:
                write_op_i32( SLAGOP_WRITE_LOCAL_X, offset );
                write_i32( slots );
                break;
            }
          }
        }
        break;

      case SLAGCMD_STATIC_CALL:
        {
          SlagMethodInfo* m  = load_method();
          load_ref_expression();

          SlagInt32 obj_offset = m->parameters->num_ref_params - 1;

          int count = m->parameters->count();
          while (count--)
          {
            load_expression();
          }

          if ( !(m->qualifiers & SLAG_QUALIFIER_EMPTY_BODY) || m->return_type || m->native_handler)
          {
            write_op_i32( SLAGOP_STATIC_CALL, m->index );
            write_i32( obj_offset );
          }
          else
          {
            write_op_i32( SLAGOP_FAUX_STATIC_CALL, m->index );
          }
          return m->return_type;
        }
        break;

      case SLAGCMD_DYNAMIC_CALL:
        {
          SlagMethodInfo* m  = load_method();
          SlagInt32 dispatch_table_offset = reader.readX();
          load_ref_expression();

          int count = m->parameters->count();
          while (count--)
          {
            load_expression();
          }

          SlagInt32 obj_offset = m->parameters->num_ref_params - 1;
          write_op_i32( SLAGOP_DYNAMIC_CALL, obj_offset );
          write_i32( dispatch_table_offset );

          return m->return_type;
        }
        break;

      case SLAGCMD_NEW_OBJECT:
        {
          SlagMethodInfo* m  = load_method();
          SlagTypeInfo* type = load_type();

          if ( !(type->method_init_object->qualifiers & SLAG_QUALIFIER_EMPTY_BODY)
              || type->method_init_object->native_handler)
          {
            write_op_i32( SLAGOP_NEW_OBJECT, type->index );
          }
          else
          {
            write_op_i32( SLAGOP_NEW_OBJECT_NO_INIT, type->index );
          }

          int count = m->parameters->count();
          while (count--)
          {
            load_expression();
          }

          if (!(m->qualifiers & SLAG_QUALIFIER_EMPTY_BODY) || m->return_type || m->native_handler)
          {
            write_op_i32( SLAGOP_STATIC_CALL, m->index );
            write_i32( m->parameters->num_ref_params - 1 );
          }
          else
          {
            if (m->parameters->count() == 0)
            {
              write_op( SLAGOP_POP_REF );
            }
            else
            {
              write_op_i32( SLAGOP_FAUX_STATIC_CALL, m->index );
            }
          }

          return type;
        }

      case SLAGCMD_NEW_COMPOUND:
        {
          SlagTypeInfo* type = load_type();

          int count = type->properties.count;
          while (count--)
          {
            load_expression();
          }

          // Just evaluating all the new compound parameters in reverse order results in a new
          // compound on the stack - no further action is required.
          return type;
        }

      case SLAGCMD_ARRAY_CREATE:
        {
          SlagTypeInfo* type = load_type();
          load_int32_expression();
          write_op_i32( SLAGOP_NEW_ARRAY, type->index );
          return type;
        }
        break;

      case SLAGCMD_ARRAY_GET:
        {
          SlagTypeInfo* context_type = load_ref_expression();
          load_int32_expression();

          if (context_type->element_type->is_reference())
          {
            write_op( SLAGOP_ARRAY_READ_REF );
          }
          else if (context_type->element_type == vm.type_real32)
          {
            write_op( SLAGOP_ARRAY_READ_REAL32 );
          }
          else
          {
            int size = context_type->element_size;

            switch (size)
            {
              case 1:
                write_op(SLAGOP_ARRAY_READ_1U);
                break;
              case 2:
                write_op(SLAGOP_ARRAY_READ_2U);
                break;
              case 4:  write_op( SLAGOP_ARRAY_READ_4 ); break;
              case 8:  write_op( SLAGOP_ARRAY_READ_8 ); break;
              default: 
                // Arrays know their own data size so we'll figure it out from that.
                write_op( SLAGOP_ARRAY_READ_X ); break;
            }
          }
          return (SlagTypeInfo*) context_type->element_type;      
        }
		

      case SLAGCMD_ARRAY_SET:
        {
          SlagTypeInfo* context_type = load_ref_expression();
          load_int32_expression(); // index expr
          load_expression(); // value expr

          if (context_type->element_type->is_reference())
          {
            write_op( SLAGOP_ARRAY_WRITE_REF );
          }
          else if (context_type->element_type == vm.type_real32)
          {
            write_op( SLAGOP_ARRAY_WRITE_REAL32 );
          }
          else
          {
            switch (context_type->element_size)
            {
              case 1:  write_op( SLAGOP_ARRAY_WRITE_1 ); break;
              case 2:  write_op( SLAGOP_ARRAY_WRITE_2 ); break;
              case 4:  write_op( SLAGOP_ARRAY_WRITE_4 ); break;
              case 8:  write_op( SLAGOP_ARRAY_WRITE_8 ); break;
              default: write_op( SLAGOP_ARRAY_WRITE_X ); break;
            }
          }
        }
        break;

      case SLAGCMD_NARROWING_CAST:
        {
          SlagTypeInfo* expr_type = load_ref_expression();
          SlagTypeInfo* to_type = load_type();
          if (expr_type != vm.type_null)
          {
            write_op_i32( SLAGOP_TYPECHECK, to_type->index );
          }
          return to_type;
        }
        break;

      case SLAGCMD_WIDENING_CAST:
        {
          load_ref_expression();
          return load_type();
        }
        break;

      case SLAGCMD_CAST_TO_INT64:
        {
          SlagTypeInfo* type = load_expression();
          if (type->is_real()) write_op( SLAGOP_CAST_REAL_TO_INTEGER );
        }
        return vm.type_int64;

      case SLAGCMD_CAST_TO_INT32:
        {
          SlagTypeInfo* type = load_expression();
          if (type->is_real()) write_op( SLAGOP_CAST_REAL_TO_INTEGER );
          else if (type == vm.type_int64) write_op( SLAGOP_CAST_INTEGER_TO_I32 );
        }
        return vm.type_int32;

      case SLAGCMD_CAST_TO_CHAR:
        {
          load_expression();
          write_op( SLAGOP_CAST_INTEGER_TO_CHAR );
        }
        return vm.type_char;

      case SLAGCMD_CAST_TO_BYTE:
        {
          load_expression();
          write_op( SLAGOP_CAST_INTEGER_TO_BYTE );
        }
        return vm.type_byte;

      case SLAGCMD_CAST_TO_REAL64:
        {
          SlagTypeInfo* type = load_expression();
          if (type->is_int32_64()) write_op( SLAGOP_CAST_INTEGER_TO_REAL );
        }
        return vm.type_real64;

      case SLAGCMD_CAST_TO_REAL32:
        {
          SlagTypeInfo* type = load_expression();
          if (type->is_int32_64()) write_op( SLAGOP_CAST_INTEGER_TO_REAL );
        }
        return vm.type_real32;

      case SLAGCMD_CAST_TO_LOGICAL:
        {
          SlagTypeInfo* type = load_expression();
          if (type == vm.type_int32)        write_op( SLAGOP_CAST_INTEGER_TO_LOGICAL );
          else if (type == vm.type_real64)  write_op( SLAGOP_CAST_REAL_TO_LOGICAL );
        }
        return vm.type_logical;

      case SLAGCMD_AS:
        {
          load_expression();
          SlagTypeInfo* of_type = load_type();
          if (of_type->is_reference())
          {
            write_op_i32( SLAGOP_AS_REF, of_type->index );
          }
          // No operation is required for non-references.
          return of_type;
        }
        break;

      case SLAGCMD_COERCE_AS:
        {
          load_expression();
          SlagTypeInfo* of_type = load_type();
          return of_type;
        }
        break;

      case SLAGCMD_CMP_INSTANCE_OF:
        {
          load_ref_expression();
          write_op_i32( SLAGOP_CMP_INSTANCE_OF, load_type()->index );
          return vm.type_logical;
        }
        break;

      case SLAGCMD_CMP_IS:
        {
          load_ref_expression();
          load_ref_expression();
          write_op( SLAGOP_CMP_EQ_REF );
        }
        return vm.type_logical;

      case SLAGCMD_CMP_IS_NOT:
        {
          load_ref_expression();
          load_ref_expression();
          write_op( SLAGOP_CMP_NE_REF );
        }
        return vm.type_logical;

      case SLAGCMD_CMP_IS_NULL:
        {
          load_ref_expression();
          write_op( SLAGOP_CMP_EQ_REF_NULL );
        }
        return vm.type_logical;

      case SLAGCMD_CMP_IS_NOT_NULL:
        {
          load_ref_expression();
          write_op( SLAGOP_CMP_NE_REF_NULL );
        }
        return vm.type_logical;

      case SLAGCMD_CMP_EQ:
        {
          SlagTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( SLAGOP_CMP_EQ_REAL );
          else if (type->is_integer()) write_op( SLAGOP_CMP_EQ_INTEGER );
          else write_op_i32( SLAGOP_CMP_EQ_X, (SlagInt32) (type->stack_slots) );
        }
        return vm.type_logical;

      case SLAGCMD_CMP_NE:
        {
          SlagTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( SLAGOP_CMP_NE_REAL );
          else if (type->is_integer()) write_op( SLAGOP_CMP_NE_INTEGER );
          else write_op_i32( SLAGOP_CMP_NE_X, (SlagInt32) (type->stack_slots) );
        }
        return vm.type_logical;

      case SLAGCMD_CMP_LE:
        {
          SlagTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( SLAGOP_CMP_LE_REAL );
          else write_op( SLAGOP_CMP_LE_INTEGER );
        }
        return vm.type_logical;

      case SLAGCMD_CMP_LT:
        {
          SlagTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( SLAGOP_CMP_LT_REAL );
          else write_op( SLAGOP_CMP_LT_INTEGER );
        }
        return vm.type_logical;

      case SLAGCMD_CMP_GT:
        {
          SlagTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( SLAGOP_CMP_GT_REAL );
          else write_op( SLAGOP_CMP_GT_INTEGER );
        }
        return vm.type_logical;

      case SLAGCMD_CMP_GE:
        {
          SlagTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( SLAGOP_CMP_GE_REAL );
          else write_op( SLAGOP_CMP_GE_INTEGER );
        }
        return vm.type_logical;

      case SLAGCMD_NOT:
        {
          SlagTypeInfo* type = load_expression();

          if (type == vm.type_logical) write_op( SLAGOP_NOT_LOGICAL );
          else                              write_op( SLAGOP_NOT_INTEGER );
          return type;
        }

      case SLAGCMD_NEGATE:
        {
          SlagTypeInfo* type = load_expression();

          if (type->is_real()) write_op( SLAGOP_NEGATE_REAL );
          else                 write_op( SLAGOP_NEGATE_INTEGER );
          return type;
        }

      case SLAGCMD_ADD:
        {
          SlagTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( SLAGOP_ADD_REAL );
          else                 write_op( SLAGOP_ADD_INTEGER );
          return type;
        }

      case SLAGCMD_SUB:
        {
          SlagTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( SLAGOP_SUB_REAL );
          else                 write_op( SLAGOP_SUB_INTEGER );
          return type;
        }

      case SLAGCMD_MUL:
        {
          SlagTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( SLAGOP_MUL_REAL );
          else                 write_op( SLAGOP_MUL_INTEGER );
          return type;
        }

      case SLAGCMD_DIV:
        {
          SlagTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( SLAGOP_DIV_REAL );
          else                 write_op( SLAGOP_DIV_INTEGER );
          return type;
        }

      case SLAGCMD_MOD:
        {
          SlagTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( SLAGOP_MOD_REAL );
          else                 write_op( SLAGOP_MOD_INTEGER );
          return type;
        }

      case SLAGCMD_EXP:
        {
          SlagTypeInfo* type = load_expression();
          load_expression();

          if (type->is_real()) write_op( SLAGOP_EXP_REAL );
          else                 write_op( SLAGOP_EXP_INTEGER );
          return type;
        }

      case SLAGCMD_AND:
        {
          SlagTypeInfo* type = load_expression();
          load_expression();

          if (type->is_logical())   write_op( SLAGOP_AND_LOGICAL );
          else                      write_op( SLAGOP_AND_INTEGER );
          return type;
        }

      case SLAGCMD_OR:
        {
          SlagTypeInfo* type = load_expression();
          load_expression();

          if (type->is_logical())   write_op( SLAGOP_OR_LOGICAL );
          else                      write_op( SLAGOP_OR_INTEGER );
          return type;
        }

      case SLAGCMD_XOR:
        {
          SlagTypeInfo* type = load_expression();
          load_expression();

          if (type->is_logical())          write_op( SLAGOP_XOR_LOGICAL );
          else if (type == vm.type_int64) write_op( SLAGOP_XOR_INT64 );
          else                             write_op( SLAGOP_XOR_INT32 );

          return type;
        }

      case SLAGCMD_LAZY_AND:
        {
          int id = get_next_structure_id();
          load_logical_expression();
          write_jump_if_true( "evaluate rhs", id, 0 );
          write_op( SLAGOP_LITERAL_INTEGER_0 );
          write_op( SLAGOP_JUMP );
          write_label_address( "end", id, 0 );

          define_label( "evaluate rhs", id, 0 );
          load_logical_expression();
          define_label( "end", id, 0 );
          close_label_id( id );

          return vm.type_logical;
        }

      case SLAGCMD_LAZY_OR:
        {
          int id = get_next_structure_id();
          load_logical_expression();
          write_jump_if_false( "evaluate rhs", id, 0 );
          write_op( SLAGOP_LITERAL_INTEGER_1 );
          write_op( SLAGOP_JUMP );
          write_label_address( "end", id, 0 );

          define_label( "evaluate rhs", id, 0 );
          load_logical_expression();
          define_label( "end", id, 0 );
          close_label_id( id );

          return vm.type_logical;
        }

      case SLAGCMD_LEFT_SHIFTED:
        {
          SlagTypeInfo* type = load_expression();
          load_int32_expression();

          if (type == vm.type_int64) write_op( SLAGOP_SHL_INT64 );
          else                        write_op( SLAGOP_SHL_INT32 );

          return type;
        }


      case SLAGCMD_RIGHT_SHIFTED:
        {
          SlagTypeInfo* type = load_expression();
          load_int32_expression();

          if (type == vm.type_int64) write_op( SLAGOP_SHR_INT64 );
          else                        write_op( SLAGOP_SHR_INT32 );

          return type;
        }


      case SLAGCMD_RIGHT_XSHIFTED:
        {
          SlagTypeInfo* type = load_expression();
          load_int32_expression();

          if (type == vm.type_int64) write_op( SLAGOP_SHRX_INT64 );
          else                        write_op( SLAGOP_SHRX_INT32 );

          return type;
        }

      case SLAGCMD_THIS_ADD_AND_ASSIGN:
        {
          SlagPropertyInfo* p = load_this_property();
          SlagTypeInfo* type = p->type;

          load_expression();

          if (type->is_real()) write_op_i32( SLAGOP_THIS_ADD_ASSIGN_R64, (SlagInt32) p->offset );
          else                 write_op_i32( SLAGOP_THIS_ADD_ASSIGN_I32, (SlagInt32) p->offset );
        }
        break;

      case SLAGCMD_THIS_SUB_AND_ASSIGN:
        {
          SlagPropertyInfo* p = load_this_property();
          SlagTypeInfo* type = p->type;

          load_expression();

          if (type->is_real()) write_op_i32( SLAGOP_THIS_SUB_ASSIGN_R64, (SlagInt32) p->offset );
          else                 write_op_i32( SLAGOP_THIS_SUB_ASSIGN_I32, (SlagInt32) p->offset );
        }
        break;

      case SLAGCMD_THIS_MUL_AND_ASSIGN:
        {
          SlagPropertyInfo* p = load_this_property();
          SlagTypeInfo* type = p->type;

          load_expression();

          if (type->is_real()) write_op_i32( SLAGOP_THIS_MUL_ASSIGN_R64, (SlagInt32) p->offset );
          else                 write_op_i32( SLAGOP_THIS_MUL_ASSIGN_I32, (SlagInt32) p->offset );
        }
        break;

      case SLAGCMD_THIS_DIV_AND_ASSIGN:
        {
          SlagPropertyInfo* p = load_this_property();
          SlagTypeInfo* type = p->type;

          write_op( SLAGOP_DUPLICATE_REF );

          if (type->object_size == 8)       write_op_i32( SLAGOP_READ_THIS_PROPERTY_8, (SlagInt32) p->offset );
          else if (type == vm.type_real32) write_op_i32( SLAGOP_READ_THIS_PROPERTY_REAL32, (SlagInt32) p->offset );
          else                              write_op_i32( SLAGOP_READ_THIS_PROPERTY_4, (SlagInt32) p->offset );

          load_expression();

          if (type->is_real()) write_op( SLAGOP_DIV_REAL );
          else                 write_op( SLAGOP_DIV_INTEGER );

          if (type->object_size == 8)       write_op_i32( SLAGOP_WRITE_THIS_PROPERTY_8, (SlagInt32) p->offset );
          else if (type == vm.type_real32) write_op_i32( SLAGOP_WRITE_THIS_PROPERTY_REAL32, (SlagInt32) p->offset );
          else                              write_op_i32( SLAGOP_WRITE_THIS_PROPERTY_4, (SlagInt32) p->offset );
        }
        break;

      case SLAGCMD_THIS_MOD_AND_ASSIGN:
        {
          SlagPropertyInfo* p = load_this_property();
          SlagTypeInfo* type = p->type;

          write_op( SLAGOP_DUPLICATE_REF );

          if (type->object_size == 8)       write_op_i32( SLAGOP_READ_THIS_PROPERTY_8, (SlagInt32) p->offset );
          else if (type == vm.type_real32) write_op_i32( SLAGOP_READ_THIS_PROPERTY_REAL32, (SlagInt32) p->offset );
          else                              write_op_i32( SLAGOP_READ_THIS_PROPERTY_4, (SlagInt32) p->offset );

          load_expression();

          if (type->is_real()) write_op( SLAGOP_MOD_REAL );
          else                 write_op( SLAGOP_MOD_INTEGER );

          if (type->object_size == 8)       write_op_i32( SLAGOP_WRITE_THIS_PROPERTY_8, (SlagInt32) p->offset );
          else if (type == vm.type_real32) write_op_i32( SLAGOP_WRITE_THIS_PROPERTY_REAL32, (SlagInt32) p->offset );
          else                              write_op_i32( SLAGOP_WRITE_THIS_PROPERTY_4, (SlagInt32) p->offset );
        }
        break;


      case SLAGCMD_LOCAL_VAR_ADD_AND_ASSIGN:
        {
          SlagLocalVarInfo* v = load_local();
          SlagTypeInfo* type = v->type;
          SlagInt32 offset = v->offset;

          load_expression();

          if (type->is_real()) write_op_i32( SLAGOP_LOCAL_ADD_ASSIGN_REAL, offset );
          else                 write_op_i32( SLAGOP_LOCAL_ADD_ASSIGN_INTEGER, offset );
        }
        break;

      case SLAGCMD_LOCAL_VAR_SUB_AND_ASSIGN:
        {
          SlagLocalVarInfo* v = load_local();
          SlagTypeInfo* type = v->type;
          SlagInt32 offset = v->offset;

          load_expression();

          if (type->is_real()) write_op_i32( SLAGOP_LOCAL_SUB_ASSIGN_REAL, offset );
          else                 write_op_i32( SLAGOP_LOCAL_SUB_ASSIGN_INTEGER, offset );
        }
        break;

      case SLAGCMD_LOCAL_VAR_MUL_AND_ASSIGN:
        {
          SlagLocalVarInfo* v = load_local();
          SlagTypeInfo* type = v->type;
          SlagInt32 offset = v->offset;

          load_expression();

          if (type->is_real()) write_op_i32( SLAGOP_LOCAL_MUL_ASSIGN_REAL, offset );
          else                 write_op_i32( SLAGOP_LOCAL_MUL_ASSIGN_INTEGER, offset );
        }
        break;

      case SLAGCMD_LOCAL_VAR_DIV_AND_ASSIGN:
        {
          SlagLocalVarInfo* v = load_local();
          SlagTypeInfo* type = v->type;
          SlagInt32 offset = v->offset;

          write_op_i32( SLAGOP_READ_LOCAL_8, offset );

          load_expression();

          if (type->is_real()) write_op( SLAGOP_DIV_REAL );
          else                 write_op( SLAGOP_DIV_INTEGER );

          write_op_i32( SLAGOP_WRITE_LOCAL_8, offset );
        }
        break;

      case SLAGCMD_LOCAL_VAR_MOD_AND_ASSIGN:
        {
          SlagLocalVarInfo* v = load_local();
          SlagTypeInfo* type = v->type;
          SlagInt32 offset = v->offset;

          write_op_i32( SLAGOP_READ_LOCAL_8, offset );

          load_expression();

          if (type->is_real()) write_op( SLAGOP_MOD_REAL );
          else                 write_op( SLAGOP_MOD_INTEGER );

          write_op_i32( SLAGOP_WRITE_LOCAL_8, offset );
        }
        break;

      case SLAGCMD_THIS_INCREMENT:
        {
          SlagPropertyInfo* p = load_this_property();
          SlagInt32 offset = p->offset;

          if (p->type->is_real()) write_op_i32( SLAGOP_THIS_INCREMENT_R64, offset );
          else                    write_op_i32( SLAGOP_THIS_INCREMENT_I32, offset );
        }
        break;

      case SLAGCMD_THIS_DECREMENT:
        {
          SlagPropertyInfo* p = load_this_property();
          SlagInt32 offset = p->offset;

          if (p->type->is_real()) write_op_i32( SLAGOP_THIS_DECREMENT_R64, offset );
          else                    write_op_i32( SLAGOP_THIS_DECREMENT_I32, offset );
        }
        break;

      case SLAGCMD_LOCAL_INCREMENT:
        {
          SlagLocalVarInfo* v = load_local();
          SlagInt32 offset = v->offset;

          if (v->type->is_real()) write_op_i32( SLAGOP_LOCAL_INCREMENT_REAL, offset );
          else                    write_op_i32( SLAGOP_LOCAL_INCREMENT_INTEGER, offset );
        }
        break;

      case SLAGCMD_LOCAL_DECREMENT:
        {
          SlagLocalVarInfo* v = load_local();
          SlagInt32 offset = v->offset;

          if (v->type->is_real()) write_op_i32( SLAGOP_LOCAL_DECREMENT_REAL, offset );
          else                    write_op_i32( SLAGOP_LOCAL_DECREMENT_INTEGER, offset );
        }
        break;

      case SLAGCMD_ARRAY_DUPLICATE:
        {
          SlagTypeInfo* array_type = load_expression();
          write_op( SLAGOP_ARRAY_DUPLICATE );
          return array_type;
        }
        break;

      default:
        {
          char buffer[80];
          sprintf( buffer, "Unhandled opcode while loading: %d.", op );
          slag_throw_fatal_error( buffer );
        }
    }
    break;

  }

  return 0;
}

SlagTypeInfo* SlagLoader::load_expression()
{
  SlagTypeInfo* result = load_statement();
  slag_assert( result != NULL, "Expression expected." );
  return result;
}

SlagTypeInfo* SlagLoader::load_ref_expression()
{
  SlagTypeInfo* result = load_statement();
  slag_assert( result && result->is_reference(), "Reference expression expected." );
  return result;
}

void SlagLoader::load_logical_expression()
{
  SlagTypeInfo* result = load_statement();
  slag_assert( result && result->is_logical(), "Logical expression expected." );
}

void SlagLoader::load_int32_expression()
{
  SlagTypeInfo* result = load_statement();
  slag_assert( result && result == vm.type_int32, "Int32 expression expected." );
}

SlagTypeInfo* SlagLoader::load_compound_expression()
{
  SlagTypeInfo* result = load_statement();
  slag_assert( result && result->is_compound(), "Int32 expression expected." );
  return result;
}

int SlagLoader::get_next_structure_id()
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

void SlagLoader::define_label( const char* name, int id, int n )
{
  SlagCodeLabel label(name,id,n);

  int address_index;
  if ( define_transient_label(name,id,n,&address_index) )
  {
    label.address_index = address_index;
  }
  else
  {
    label.address_index = vm.address_offsets.count;
    vm.address_offsets.add( vm.code.count );
  }

  resolved_labels.add(label);
}

bool SlagLoader::define_transient_label( const char* name, int id, int n, int* index_ptr )
{
  SlagCodeLabel label(name,id,n);

  // Backpatch any unresolved references to this label.  They will all
  // point to the same address table entry, so fixing one will fix
  // all - as such, only one backpatch request is actually saved.
  for (int i=0; i<unresolved_labels.count; ++i)
  {
    if (unresolved_labels[i].equals(label))
    {
      label.address_index = unresolved_labels[i].address_index;
      if (index_ptr) *index_ptr = label.address_index;
      vm.address_offsets[label.address_index] = vm.code.count;
      unresolved_labels.remove(i);
      return true;
    }
  }

  return false;
}

void SlagLoader::write_label_address( const char* name, int id, int n )
{
  SlagCodeLabel label(name,id,n);

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
  label.address_index = vm.address_offsets.count;
  vm.address_offsets.add(0);
  unresolved_labels.add( label );

  write_op(label.address_index);
}

void SlagLoader::close_label_id( int id )
{
  // Ensure every label in the backpatch list has been taken care of.
  for (int i=0; i<unresolved_labels.count; ++i)
  {
    bool result = (unresolved_labels[i].id == id);
    slag_assert( result==false, "Pending label closed without backpatch." );
  }

  while (resolved_labels.count && resolved_labels.last().id == id)
  {
    resolved_labels.remove_last();
  }
}

void SlagLoader::write_jump_if_true( const char* name, int id, int n )
{
  int op = SLAGOP_JUMP_IF_TRUE;
  if (history(0) == SLAGOP_CMP_EQ_REF_NULL)
  {
    undo_op();
    op = SLAGOP_JUMP_IF_NULL_REF;
  }
  else if (history(0) == SLAGOP_CMP_NE_REF_NULL)
  {
    undo_op();
    op = SLAGOP_JUMP_IF_REF;
  }

  write_op( op );
  write_label_address( name, id, n );
}

void SlagLoader::write_jump_if_false( const char* name, int id, int n )
{
  int op = SLAGOP_JUMP_IF_FALSE;
  if (history(0) == SLAGOP_CMP_EQ_REF_NULL)
  {
    undo_op();
    op = SLAGOP_JUMP_IF_REF;
  }
  else if (history(0) == SLAGOP_CMP_NE_REF_NULL)
  {
    undo_op();
    op = SLAGOP_JUMP_IF_NULL_REF;
  }
  write_op( op );
  write_label_address( name, id, n );
}

void slag_set_raw_exe_filepath( char* filepath )
{
  vm.raw_exe_filepath = filepath;
}

void slag_set_command_line_args( char** argv, int argc )
{
  vm.set_command_line_args( argv, argc );
}

void slag_throw_file_error()
{
  vm.throw_exception( vm.type_file_error );
}

void slag_throw_file_error( char* filename )
{
  vm.throw_exception( vm.type_file_error, filename );
}

void slag_throw_file_not_found_error( char* filename )
{
  vm.throw_exception( vm.type_file_not_found_error, filename );
}

void slag_throw_no_next_value_error()
{
  vm.throw_exception( vm.type_no_next_value_error );
}

void slag_throw_socket_error()
{
  vm.throw_exception( vm.type_socket_error );
}

void slag_throw_invalid_operand_error()
{
  vm.throw_exception( vm.type_invalid_operand_error );
}

