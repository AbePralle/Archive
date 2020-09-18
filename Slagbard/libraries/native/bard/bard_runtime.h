#ifndef BARD_RUNTIME_H
#define BARD_RUNTIME_H
//=============================================================================
//  bard_runtime.h
//
//  Runtime info common to both VM and XC builds.
//
//  $(BARD_VERSION)
//  ---------------------------------------------------------------------------
//
//  Copyright 2008-2011 Plasmaworks LLC
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
//
//  ---------------------------------------------------------------------------
//
//  History:
//    2010.12.25 / Abe Pralle - Created
//=============================================================================

#define BARD_PUSH_REAL64  BARD_PUSH_REAL
#define BARD_PUSH_REAL32  BARD_PUSH_REAL
#define BARD_PUSH_INT64   BARD_PUSH_INTEGER
#define BARD_PUSH_INT32   BARD_PUSH_INTEGER
#define BARD_PUSH_CHAR    BARD_PUSH_INTEGER
#define BARD_PUSH_BYTE    BARD_PUSH_INTEGER
#define BARD_PUSH_LOGICAL BARD_PUSH_INTEGER

#define BARD_POP_REAL64()  BARD_POP_REAL()
#define BARD_POP_REAL32()  ((BardReal32)BARD_POP_REAL())
#define BARD_POP_INT64()   BARD_POP_INTEGER()
#define BARD_POP_INT32()   ((BardInt32)BARD_POP_INTEGER())
#define BARD_POP_CHAR()    ((BardChar)BARD_POP_INTEGER())
#define BARD_POP_BYTE()    ((BardByte)BARD_POP_INTEGER())
#define BARD_POP_LOGICAL() (BARD_POP_INTEGER() != 0)

#define BARD_GET_REAL64( var_name, obj, name ) BARD_GET(BardReal64,var_name,obj,name)
#define BARD_GET_REAL32( var_name, obj, name ) BARD_GET(BardReal32,var_name,obj,name)
#define BARD_GET_INT64( var_name, obj, name ) BARD_GET(BardInt64,var_name,obj,name)
#define BARD_GET_INT32( var_name, obj, name ) BARD_GET(BardInt32,var_name,obj,name)
#define BARD_GET_CHAR( var_name, obj, name ) BARD_GET(BardChar,var_name,obj,name)
#define BARD_GET_BYTE( var_name, obj, name ) BARD_GET(BardByte,var_name,obj,name)
#define BARD_GET_LOGICAL( var_name, obj, name ) BARD_GET(BardLogical,var_name,obj,name)

#define BARD_GET_NATIVE_DATA( type, varname, context, property_name ) \
  type varname = NULL; \
  { \
    BARD_GET( BardNativeData*, _native_data_obj_temp, context, property_name ); \
    if ( _native_data_obj_temp ) varname = (type) _native_data_obj_temp->data; \
  }

#define BARD_RELEASE_NATIVE_DATA( context, property_name ) \
  { \
    BARD_GET( BardNativeData*, _native_data_obj_temp, context, property_name ); \
    if (_native_data_obj_temp) \
    { \
      BARD_SET_REF( context, property_name, NULL ); \
      _native_data_obj_temp->release(); \
    } \
  }

#define BARD_SET_REAL64( obj, name, value ) BARD_SET(BardReal64,obj,name,value)
#define BARD_SET_REAL32( obj, name, value ) BARD_SET(BardReal32,obj,name,value)
#define BARD_SET_INT64( obj, name, value )  BARD_SET(BardInt64,obj,name,value)
#define BARD_SET_INT32( obj, name, value )  BARD_SET(BardInt32,obj,name,value)
#define BARD_SET_CHAR( obj, name, value )   BARD_SET(BardChar,obj,name,value)
#define BARD_SET_BYTE( obj, name, value )   BARD_SET(BardByte,obj,name,value)
#define BARD_SET_LOGICAL( obj, name,value ) BARD_SET(BardLogical,obj,name,value)

#define BARD_QUALIFIER_CLASS         1
#define BARD_QUALIFIER_ASPECT       (1 << 1)
#define BARD_QUALIFIER_PRIMITIVE    (1 << 2)
#define BARD_QUALIFIER_COMPOUND     (1 << 3)
#define BARD_QUALIFIER_SINGLETON    (1 << 4)
#define BARD_QUALIFIER_MANAGED      (1 << 5)
#define BARD_QUALIFIER_DEFERRED     (1 << 6)
#define BARD_QUALIFIER_UNDERLYING   (1 << 7)
#define BARD_QUALIFIER_OVERLAYING   (1 << 8)
#define BARD_QUALIFIER_REQUISITE    (1 << 9)
#define BARD_QUALIFIER_LIMITED      (1 <<10)
#define BARD_QUALIFIER_NATIVE       (1 <<11)
#define BARD_QUALIFIER_ABSTRACT     (1 <<12)
#define BARD_QUALIFIER_AUTOMATIC    (1 <<13)
#define BARD_QUALIFIER_PROPAGATED   (1 <<14)
#define BARD_QUALIFIER_PUBLIC       (1 <<15)
#define BARD_QUALIFIER_PRIVATE      (1 <<16)
#define BARD_QUALIFIER_READONLY     (1 <<17)
#define BARD_QUALIFIER_WRITEONLY    (1 <<18)
#define BARD_QUALIFIER_CONTAINS_THROW (1 <<19)
#define BARD_QUALIFIER_EMPTY_BODY     (1 <<20)
#define BARD_QUALIFIER_AUGMENT          (1<<21)
#define BARD_QUALIFIER_ENUM             (1<<22)
#define BARD_QUALIFIER_GENERIC          (1<<23)
#define BARD_QUALIFIER_CONSTANT         (1<<24)
#define BARD_QUALIFIER_REQUIRES_CLEANUP (1<<25)
#define BARD_QUALIFIER_ARRAY            (1<<26)
#define BARD_QUALIFIER_REFERENCE_ARRAY  (1<<27)
#define BARD_QUALIFIER_RUNTIME          (1<<28)

#define BARD_QUALIFIER_REFERENCE  (BARD_QUALIFIER_CLASS | BARD_QUALIFIER_ASPECT)
#define BARD_QUALIFIER_KIND       (BARD_QUALIFIER_REFERENCE | BARD_QUALIFIER_PRIMITIVE | BARD_QUALIFIER_COMPOUND)

extern ASCIIString bard_error_message;

struct BardTypeInfo;

struct BardObject
{
  BardTypeInfo* type;
  BardObject*   next;
  BardInt32     reference_count;

};

struct BardArray : BardObject
{
  BardInt32 array_count;
  BardInt64 data[1];  // Int64 to give this member 8-byte alignment.
};

struct BardString : BardObject
{
  static BardString* create( int count );
  static BardString* create( BardChar* data, int count );
  static BardString* create( const char* data, int count=-1 );
  static BardTypeInfo* string_type();

  BardInt32     count;
  BardInt32     hash_code;
  BardChar      characters[1];

  void  set_hash_code();
  void  to_ascii( char* buffer, int buffer_len );
  char* to_new_ascii();
};

struct BardWeakRef : BardObject
{
  static BardWeakRef* create( BardObject* obj );

  BardObject* object;

  BardWeakRef( BardObject* object );
  void set( BardObject* object );
};


struct BardArrayList : BardObject
{
  BardArray* array;
  BardInt32  count;
  BardInt32  modification_count;
};

struct BardParseReader : BardObject
{
  BardInt32  property_line, property_column, property_pos, property_remaining;
  BardInt32  property_spaces_per_tab;
  BardArray* property_data;
};

struct BardException : BardObject
{
  BardString* message;
  BardObject* stack_trace;
};

struct BardDate : BardObject
{
  BardInt32 year;
  BardInt32 month;
  BardInt32 day;
  BardInt32 hour;
  BardInt32 minute;
  BardInt32 second;
  BardInt32 millisecond;
};

struct BardFile : BardObject
{
  BardString* property_filepath;
};

struct BardFileReader : BardObject
{
  BardObject* property_native_data;
};

struct BardFileWriter : BardObject
{
  BardObject* property_native_data;
};

typedef void (*BardNativeDataDeleteFn)(void*);

void BardNativeDataNoDelete( void* data );
void BardNativeDataGenericDelete( void* data );
void BardNativeDataDeleteResource( void* data );

struct BardNativeData : BardObject
{
  static BardNativeData* create( void* data, BardNativeDataDeleteFn delete_fn );

  void* data;
  BardNativeDataDeleteFn delete_fn;

  void init( void* data, BardNativeDataDeleteFn delete_fn )
  {
    this->data = data;
    this->delete_fn = delete_fn;
  }

  ~BardNativeData()
  {
    release();
  }

  void release()
  {
    if (data)
    {
      // Set pointer to null before calling delete function avoid any chance of
      // calling it twice.
      void* ptr = data;
      data = 0;
      delete_fn(ptr);
    }
  }
};


struct BardResource
{
  virtual ~BardResource() { }
};

struct BardNativeDataWrapper : BardObject
{
  BardNativeData* native_data;
};

struct BardSocket : BardObject
{
  BardNativeData* native_data;
  BardString*     address;
  BardInt32       port;
  BardNativeDataWrapper *reader;
  BardNativeDataWrapper *writer;
  // There are more properties but these are all we need on the native side.
};

//=============================================================================
//  BardTypeInfoBase
//=============================================================================
struct BardTypeInfoBase
{
  int qualifiers;
  int index;
  int singleton_index;
  int object_size;  // size of actual data
  int element_size; // byte size of an array element (if array)

  char* name;

  ArrayList<BardTypeInfoBase*> base_types;
  ArrayList<int>               reference_property_offsets;

  BardTypeInfoBase() : singleton_index(0), element_size(0)
  {
  }

  inline bool is_reference() { return (qualifiers & BARD_QUALIFIER_REFERENCE) != 0; }
  inline bool is_primitive() { return (qualifiers & BARD_QUALIFIER_PRIMITIVE) != 0; }
  inline bool is_singleton() { return (qualifiers & BARD_QUALIFIER_SINGLETON) != 0; }
  inline bool is_managed() { return (qualifiers & BARD_QUALIFIER_MANAGED) != 0; }
  inline bool is_compound()  { return (qualifiers & BARD_QUALIFIER_COMPOUND)  != 0; }
  inline bool requires_cleanup() { return (qualifiers & BARD_QUALIFIER_REQUIRES_CLEANUP) != 0; }
  inline bool is_array() { return (qualifiers & BARD_QUALIFIER_ARRAY) != 0; }
  inline bool is_reference_array() { return (qualifiers & BARD_QUALIFIER_REFERENCE_ARRAY) != 0; }

  bool instance_of( BardTypeInfoBase* base_type )
  {
    if (this == base_type) return true;

    int count = base_types.count + 1;
    BardTypeInfoBase** base_types = this->base_types.data - 1;
    while (--count)
    {
      if (base_type == *(++base_types)) return true;
    }

    return false;
  }
};


void bard_shut_down();
void bard_init();
void bard_configure();
void bard_launch();

void bard_throw_fatal_error( const char* st );
void bard_throw_fatal_error( const char* st1, const char* st2, const char* st3 );
void bard_throw_fatal_error( const char* st1, const char* st2, const char* st3,
    const char* st4, const char* st5 );

typedef void (*BardNativeFn)();
void bard_register_native_method( const char* class_name, const char* method_name, BardNativeFn fn_ptr );

extern ArrayList<BardNativeFn> bard_custom_shutdown_functions;
void  bard_add_custom_shutdown( BardNativeFn fn );

BardArray*     bard_create_byte_array( char* data, int count );
BardArrayList* bard_create_byte_list( char* data, int count );
BardArray*     bard_create_char_array( BardChar* data, int count );
BardArrayList* bard_create_char_list( BardChar* data, int count );
BardArray*     bard_duplicate_array( BardObject* obj );

#ifdef BARD_USE_LONGJMP
  struct BardCatchHandler
  {
    BardCatchHandler* prev;
    jmp_buf env;

    BardCatchHandler();
    ~BardCatchHandler();
    void unhook();
  };

  extern jmp_buf bard_fatal_jumppoint;
  extern BardCatchHandler* bard_cur_catch;;
# define BARD_TRY if (!setjmp(bard_cur_catch->env))
# define BARD_CATCH(err_name) else
# define BARD_THROW(err) bard_thrown_error = err; longjmp( bard_cur_catch->env, 1 )

#else
# define BARD_TRY try
# define BARD_CATCH(err_name) catch (BardObject* err_name)
# define BARD_THROW(err) throw err
#endif

extern BardObject* bard_thrown_error;


#include "bard_mm.h"
#if defined(BARD_VM)
#  include "bard_vm.h"
#else
#  include "bard_xc.h"
#endif
#include "bard_stdlib.h"

extern BardGlobalRef bard_language_ref;
extern BardGlobalRef bard_os_ref;
extern BardGlobalRef bard_os_version_ref;
extern BardGlobalRef bard_hw_version_ref;

#define BARD_WRITE_REF( old_ref, new_ref ) \
  if (old_ref) --old_ref->reference_count; \
  old_ref = new_ref; \
  if (new_ref) ++new_ref->reference_count;

#endif // BARD_RUNTIME_H
