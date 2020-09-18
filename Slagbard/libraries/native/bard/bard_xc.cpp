//=============================================================================
//  bard_xc.cpp
//
//  Cross-Compiled Bard Routines
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

#include "bard.h"

extern BardTypeInfo type_ClassObject;
extern BardTypeInfo type_ClassString;
extern BardTypeInfo type_ClassGlobal;
extern BardTypeInfo type_ClassSocketError;
extern BardTypeInfo type_ClassDivideByZeroError;
extern BardTypeInfo type_ClassInvalidOperandError;
BardObject* sxc_get_global_cmd_line_args_list();

//=============================================================================
//  Global Variables
//=============================================================================
BardXC sxc;
char* sxc_raw_exe_filepath = NULL;
BardTypeInfo* sxc_main_class = NULL;

//=============================================================================
//  BardXC
//=============================================================================
void BardXC::shut_down()
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
}

void BardXC::set_up_method_info( int* info_table, int count )
{
  for (int i=0; i<count; ++i)
  {
    BardMethodInfo* m = &sxc_methods[i];
    m->name = sxc_identifiers[*(info_table++)];
    m->parameter_signature = sxc_identifiers[*(info_table++)];
    m->method_ptr = sxc_method_pointers[i];
    m->method_caller = sxc_caller_table[*(info_table++)];
    m->index = i;
  }
}

void BardXC::create_method_lists( int* info_table, int num_lists )
{
  for (int i=0; i<num_lists; ++i)
  {
    BardTypeInfo* type = sxc_types[ *(info_table++) ];
    int m_count = *(info_table++);
    type->methods.clear();
    type->methods.ensure_capacity( m_count );
    for (int j=0; j<m_count; ++j)
    {
      type->methods.add( &sxc_methods[ *(info_table++) ] );
    }
  }
}

BardTypeInfo* BardXC::find_type( const char* name )
{
  for (int i=0; i<sxc_types_count; ++i)
  {
    BardTypeInfo* type = sxc_types[i];
    if (type && 0 == strcmp(name,type->name))
    {
      return type;
    }
  }
  return NULL;
}

BardTypeInfo* BardXC::must_find_type( const char* name )
{
  BardTypeInfo* result = find_type(name);
  if (result == NULL) bard_throw_fatal_error( "No such type \"", name, "\"" );
  return result;
}

//=============================================================================
//  BardTypeInfo
//=============================================================================
BardObject* BardTypeInfo::create()
{
   BardObject* object = mm.create_object( this, object_size );
   if (method_init_object) method_init_object( object );
   return object;
}

BardObject* BardTypeInfo::create_without_init()
{
   BardObject* object = mm.create_object( this, object_size );
   return object;
}

BardArray* BardTypeInfo::create( int count )
{
   return mm.create_array( this, count );
}


//=============================================================================
//  BardString
//=============================================================================
BardTypeInfo* BardString::string_type()
{
  return &type_ClassString;
}

//=============================================================================
//  Miscellaneous Methods
//=============================================================================
void bard_set_raw_exe_filepath( char* filepath )
{
  sxc_raw_exe_filepath = filepath;
}

void bard_set_command_line_args( char** argv, int argc )
{
  for (int i=0; i<argc; ++i)
  {
    ArrayList_of_Object__add__Object( 
        sxc_get_global_cmd_line_args_list(), 
        BardString::create(argv[i])
      );

  }
}

BardObject* FileError__init( BardObject* context );
BardObject* FileError__init__String( BardObject* context, BardObject* filename );
BardObject* FileNotFoundError__init__String( BardObject* context, BardObject* filename );
BardObject* NoNextValueError__init( BardObject* context );
BardObject* SocketError__init( BardObject* context );
BardObject* DivideByZeroError__init( BardObject* context );
BardObject* InvalidOperandError__init( BardObject* context );

void bard_throw_file_error()
{
  BARD_THROW( FileError__init( type_ClassFileError.create() ) );
}

void bard_throw_file_error( char* filename )
{
  BARD_THROW( FileError__init__String(
      type_ClassFileError.create(), BardString::create(filename) ) );
}

void bard_throw_file_not_found_error( char* filename )
{
  BARD_THROW( FileNotFoundError__init__String(
      type_ClassFileNotFoundError.create(), BardString::create(filename) ) );
}

void bard_throw_no_next_value_error()
{
  BARD_THROW( NoNextValueError__init( type_ClassNoNextValueError.create() ) );
}

void bard_throw_socket_error()
{
  BARD_THROW( SocketError__init( type_ClassSocketError.create() ) );
}

void bard_throw_divide_by_zero_error()
{
  BARD_THROW( DivideByZeroError__init( type_ClassDivideByZeroError.create() ) );
}

void bard_throw_invalid_operand_error()
{
  BARD_THROW( InvalidOperandError__init( type_ClassInvalidOperandError.create() ) );
}


void sxc_write_ref( BardObject** dest, BardObject* obj )
{
  if (*dest) --((*dest)->reference_count);
  if (obj) ++(obj->reference_count);
  *dest = obj;
}

BardObject* sxc_as( BardObject* obj, BardTypeInfo* as_type )
{
  if (obj && obj->type->instance_of(as_type)) return obj;
  return NULL;
}

BardLogical sxc_instance_of( BardObject* obj, BardTypeInfo* of_type )
{
  if (obj && obj->type->instance_of(of_type)) return (BardLogical) 1;
  return (BardLogical) 0;
}

BardInt64 sxc_real64_as_int64( BardReal64 n )
{
  return *((BardInt64*)&n);
}

BardInt32 sxc_real32_as_int32( BardReal32 n )
{
  return *((BardInt32*)&n);
}

BardReal64 sxc_int64_as_real64( BardInt64 n )
{
  return *((BardReal64*)&n);
}

BardReal32 sxc_int32_as_real32( BardInt32 n )
{
  return *((BardReal32*)&n);
}

BardReal64  sxc_abs( BardReal64 n )
{
  return (n >= 0) ? n : -n;
}

BardReal64  sxc_mod( BardReal64 a, BardReal64 b )
{
  BardReal64 q = a / b;
  return (a - (floor(q)) * b);
}

BardReal32  sxc_mod( BardReal32 a, BardReal32 b )
{
  BardReal32 q = a / b;
  return (a - (floor(q)) * b);
}

BardInt64   sxc_mod( BardInt64 a, BardInt64 b )
{
  if (b == 0)
  {
    bard_throw_divide_by_zero_error();
  }

  if (b == 1)
  {
    return 0;
  }
  else if ((a ^ b) < 0)
  {
    BardInt64 r = a % b;
    return (r ? (r+b) : r);
  }
  else
  {
    return (a % b);
  }
}

BardInt32   sxc_mod( BardInt32 a, BardInt32 b )
{
  if (b == 0)
  {
    bard_throw_divide_by_zero_error();
  }

  if (b == 1)
  {
    return 0;
  }
  else if ((a ^ b) < 0)
  {
    BardInt32 r = a % b;
    return (r ? (r+b) : r);
  }
  else
  {
    return (a % b);
  }
}

BardInt64   sxc_shr( BardInt64 n, BardInt32 bits )
{
  if (bits == 0) return n;

  n = (n >> 1) & 0x7fffFFFFffffFFFFLL;
  if (--bits == 0) return n;

  return n >> bits;
}

BardInt32   sxc_shr( BardInt32 n, BardInt32 bits )
{
  if (bits == 0) return n;

  n = (n >> 1) & 0x7fffFFFF;
  if (--bits == 0) return n;

  return n >> bits;
}

