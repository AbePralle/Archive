#ifndef BARD_XC_H
#define BARD_XC_H
//=============================================================================
//  bard_xc.h
//
//  Cross-Compiled Bard Header
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
//    2010.12.25 / Abe Pralle - Created for v3.2 revamp
//=============================================================================

#define BARD_PUSH_REF( obj ) sxc.ref_stack_ptr[-1] = (obj); --sxc.ref_stack_ptr
#define BARD_PEEK_REF() (*(sxc.ref_stack_ptr))
#define BARD_POP_REF() *(sxc.ref_stack_ptr++)
#define BARD_DISCARD_REF() sxc.ref_stack_ptr++
#define BARD_DUPLICATE_REF() sxc.ref_stack_ptr[-1] = *sxc.ref_stack_ptr; --sxc.ref_stack_ptr

#define BARD_POP_INTEGER()   *(sxc.data_stack_ptr++)
#define BARD_PUSH_INTEGER(i) *(--sxc.data_stack_ptr) = i
#define BARD_PEEK_INTEGER() *(sxc.data_stack_ptr)

#define BARD_POP_REAL()   *(BardReal64*)(sxc.data_stack_ptr++)
#define BARD_PUSH_REAL(r) *(BardReal64*)(--sxc.data_stack_ptr) = r
#define BARD_PEEK_REAL() *(BardReal64*)(sxc.data_stack_ptr)

#define BARD_PUSH(type,value) *((type*)(sxc.data_stack_ptr -= sizeof(type)/8)) = (value)
#define BARD_PEEK(type) *((type*)sxc.data_stack_ptr)
#define BARD_POP(type) ((type*)(sxc.data_stack_ptr += sizeof(type)/8))[-1]


#define BARD_TYPE_STRING &type_ClassString
#define BARD_TYPE_WEAK_REFERENCE &type_ClassWeakReference
#define BARD_TYPE_ARRAY_OF_CHAR (&type_ClassArray_of_Char)

#define BARD_REF_STACK_PTR sxc.ref_stack_ptr
#define BARD_REF_STACK_LIMIT sxc.ref_stack_limit
#define BARD_SINGLETONS sxc_singletons
#define BARD_SINGLETONS_COUNT sxc_singletons_count

#define BARD_CALL( type, sig ) \
  {\
    static int method_index = -1; \
    if (method_index == -1) method_index = type->must_find_method(sig)->index; \
    BardMethodInfo* _m_info = &sxc_methods[method_index]; \
    ((void(*)(void*))_m_info->method_caller)(_m_info->method_ptr); \
  }

// Null check is defined out
#define BVM_NULL_CHECK( expr, cmd )

#define BARD_FIND_TYPE( var_name, type_name ) \
  BardTypeInfo* var_name; \
  { \
    static int type_index = -1; \
    if (type_index == -1) type_index = sxc.must_find_type(type_name)->index; \
    var_name = sxc_types[type_index]; \
  }

#define BARD_GET_REF(var_name,obj,name) \
    BARD_GET(BardObject*,var_name,obj,name);

#define BARD_SET_REF(obj,name,value) \
  {\
    static int property_offset = -1; \
    if (property_offset == -1) property_offset = (obj)->type->must_find_property(name)->offset; \
    BardObject** _dest_ptr = (BardObject**)(((char*)(obj)) + property_offset); \
    if (*_dest_ptr) --(*_dest_ptr)->reference_count; \
    *_dest_ptr = value; \
    if (value) ++(*_dest_ptr)->reference_count; \
  }

#define BARD_GET(vartype,var_name,obj,name) \
  vartype var_name; \
  {\
    static int property_offset = -1; \
    if (property_offset == -1) property_offset = (obj)->type->must_find_property(name)->offset; \
    var_name = *((vartype*)(((char*)(obj)) + property_offset)); \
  }

#define BARD_SET(vartype,obj,name,value) \
  {\
    static int property_offset = -1; \
    if (property_offset == -1) property_offset = (obj)->type->must_find_property(name)->offset; \
    *((vartype*)(((char*)(obj)) + property_offset)) = (vartype) value; \
  }

struct BardMethodInfo;

extern BardTypeInfo*   sxc_types[];
extern int             sxc_types_count;
extern BardObject*     sxc_singletons[];
extern int             sxc_singletons_count;
extern char*           sxc_raw_exe_filepath;
extern const char*     sxc_identifiers[];
extern BardTypeInfo*   sxc_main_class;
extern void*           sxc_method_pointers[];
extern BardMethodInfo  sxc_methods[];
extern void*           sxc_caller_table[];

//extern BardLiteralString* sxc_literal_strings;
//static int sxc_literal_strings_count = 0;

struct BardXC
{
  BardObject** ref_stack;
  BardObject** ref_stack_ptr;
  BardObject** ref_stack_limit;

  BardInt64*   data_stack;
  BardInt64*   data_stack_ptr;
  BardInt64*   data_stack_limit;

  bool initialized;

  BardXC()
  {
    initialized = false;
    init();
  }

  ~BardXC()
  {
    shut_down();
  }

  void init()
  {
    if (initialized) return;
    initialized = true;

    ref_stack = new BardObject*[BARD_STACK_SIZE];
    ref_stack_limit = ref_stack + BARD_STACK_SIZE;
    ref_stack_ptr = ref_stack_limit;

    // The data stack is only used when calling native methods.
    data_stack = new BardInt64[512];
    data_stack_limit = data_stack + 512;
    data_stack_ptr = data_stack_limit;

    for (int i=0; i<sxc_singletons_count; ++i)
    {
      sxc_singletons[i] = NULL;
    }
  }

  void shut_down();

  void set_up_method_info( int* info_table, int count );
  void create_method_lists( int* info_table, int count );

  BardTypeInfo* find_type( const char* name );
  BardTypeInfo* must_find_type( const char* name );

};

extern BardXC sxc;

typedef void (*BardFn)(BardObject*);

struct BardPropertyInfo
{
  BardTypeInfo* type;
  const char*   name;
  int           offset;

  BardPropertyInfo() { }

  BardPropertyInfo( BardTypeInfo* type, const char* name, int offset )
    : type(type), name(name), offset(offset)
  {
  }
};

struct BardMethodInfo
{
  const char* name;
  const char* parameter_signature;
  void* method_ptr;
  void* method_caller;
  int   index;

  BardMethodInfo() { }
};

struct BardTypeInfo : BardTypeInfoBase
{
  BardFn method_init_object;
  BardFn method_clean_up;
  ArrayList<BardPropertyInfo> properties;
  ArrayList<BardMethodInfo*>  methods;

  void** dispatch_table;

  BardTypeInfo( int name_index, int qualifiers, int index, int singleton_index, 
      int base_type_count, int object_size,
      int reference_property_count )
    : method_init_object(NULL), method_clean_up(NULL), dispatch_table(NULL)
  {
    this->qualifiers = qualifiers;
    this->index = index;
    this->singleton_index = singleton_index;
    this->object_size = object_size;
    name = (char*) sxc_identifiers[name_index];
    sxc_types[index] = this;
    base_types.ensure_capacity( base_type_count );
    reference_property_offsets.ensure_capacity( reference_property_count );
  }

  void set_base_types( int* table, int offset )
  {
    base_types.clear();
    for (int i=0; i<base_types.capacity; ++i)
    {
      base_types.add( sxc_types[ table[offset+i] ] );
    }
  }

  void set_property_info( int* table, int index, int count )
  {
    int offset;
    if (this->is_reference()) offset = sizeof(BardObject);
    else offset = 0;

    properties.clear();
    properties.ensure_capacity(count);
    reference_property_offsets.clear();

    for (int i=0; i<count; ++i)
    {
      BardTypeInfo* type = sxc_types[ table[index++] ];
      const char*   name = sxc_identifiers[ table[index++] ];
      int size;
      if (type->is_reference())  size = sizeof(void*);
      else size = type->object_size;
      if ((offset & 1) && size >= 2) ++offset;
      if ((offset & 2) && size >= 4) offset += 2;

      if (offsetof(BardAlignmentInfo,real)==8 && (offset & 4) && size >= 8) offset += 4;

      properties.add( BardPropertyInfo( type, name, offset ) );

      if (type->is_reference()) reference_property_offsets.add( offset );

      offset += size;
    }
  }

  BardObject* create();
  BardObject* create_without_init();
  BardArray*  create( int count );

  BardObject* singleton() { return sxc_singletons[singleton_index]; }

  BardPropertyInfo* find_property( const char* name )
  {
    for (int i=0; i<properties.count; ++i)
    {
      if (0 == strcmp(name,properties[i].name))
      {
        return &properties[i];
      }
    }
    return NULL;
  }

  BardPropertyInfo* must_find_property( const char* name )
  {
    BardPropertyInfo* result = find_property(name);
    if (result == NULL) bard_throw_fatal_error( "No such property \"", name, "\"" );
    return result;
  }

  BardMethodInfo* find_method( const char* sig )
  {
    int pos;
    for (pos=0; sig[pos]; ++pos)
    {
      if (sig[pos] == '(') break;
    }

    if (sig[pos] == 0)
    {
      bard_throw_fatal_error( "Signature string is missing parens: \"", sig, "\"." );
    }

    for (int i=0; i<methods.count; ++i)
    {
      BardMethodInfo* m = methods[i];
      if (0 == strncmp(sig,m->name,pos) && 0 == strcmp(sig+pos,m->parameter_signature))
      {
        return m;
      }
    }
    return NULL;
  }

  BardMethodInfo* must_find_method( const char* sig )
  {
    BardMethodInfo* result = find_method(sig);
    if (result == NULL) bard_throw_fatal_error( "No such method \"", sig, "\"" );
    return result;
  }
};

extern BardTypeInfo type_ClassObject;
extern BardTypeInfo type_ClassString;
extern BardTypeInfo type_ClassArray_of_Char;
extern BardTypeInfo type_ClassFileError;
extern BardTypeInfo type_ClassFileNotFoundError;
extern BardTypeInfo type_ClassNoNextValueError;
struct ClassString;

struct BardLiteralString
{
  BardString* value;

  BardLiteralString( const char* utf8, int original_len )
  {
    // Don't use BardString::create() since there's no guarantee
    // the memory manager has been set up yet.
    value = (BardString*) new char[ sizeof(BardString)-2 + original_len*2 ];
    value->type = BARD_TYPE_STRING;
    value->reference_count = 0;
    value->next = NULL;
    value->count = original_len;

    --utf8;
    for (int i=0; i<original_len; ++i)
    {
      value->characters[i] = (BardChar) read_utf8( &utf8 );
    }

    value->set_hash_code();
  }

  ~BardLiteralString()
  {
    delete value;
    value = NULL;
  }

  operator BardObject*()
  {
    return (BardObject*) value;
  }

  int read_utf8( const char** ptrptr )
  {
    int ch = *(++(*ptrptr));

    if ((ch & 0x80) != 0)
    {
      int ch2 = *(++(*ptrptr));

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
        int ch3 = *(++(*ptrptr));
        ch  &= 15;
        ch2 &= 0x3f;
        ch3 &= 0x3f;
        ch = (ch << 12) | (ch2 << 6) | ch3;
      }
    }

    return ch;
  }
};

void bard_set_raw_exe_filepath( char* filepath );
void sxc_configure();
void bard_set_command_line_args( char** argv, int argc );
void bard_throw_file_error();
void bard_throw_file_error( char* filename );
void bard_throw_file_not_found_error( char* filename );
void bard_throw_no_next_value_error();
void bard_throw_socket_error();
void bard_throw_divide_by_zero_error();
void bard_throw_invalid_operand_error();

void        sxc_write_ref( BardObject** dest, BardObject* obj );
BardObject* sxc_as( BardObject* obj, BardTypeInfo* as_type );
BardLogical sxc_instance_of( BardObject* obj, BardTypeInfo* of_type );
BardInt64   sxc_real64_as_int64( BardReal64 );
BardInt32   sxc_real32_as_int32( BardReal32 );
BardReal64  sxc_int64_as_real64( BardInt64 );
BardReal32  sxc_int32_as_real32( BardInt32 );
BardReal64  sxc_abs( BardReal64 n );
BardReal64  sxc_mod( BardReal64 a, BardReal64 b );
BardReal32  sxc_mod( BardReal32 a, BardReal32 b );
BardInt64   sxc_mod( BardInt64 a, BardInt64 b );
BardInt32   sxc_mod( BardInt32 a, BardInt32 b );
BardInt64   sxc_shr( BardInt64 n, BardInt32 bits );
BardInt32   sxc_shr( BardInt32 n, BardInt32 bits );

BardObject* ArrayList_of_Object__add__Object( BardObject* context, BardObject* object );

#endif // BARD_XC_H

