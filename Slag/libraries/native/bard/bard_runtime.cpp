//=============================================================================
//  bard_runtime.cpp
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
//    2010.12.24 / Abe Pralle - v3.2 revamp
//    2008.12.17 / Abe Pralle - Created
//=============================================================================

#include "bard.h"

//=============================================================================
//  Globals
//=============================================================================
ASCIIString bard_error_message;
BardGlobalRef bard_language_ref;
BardGlobalRef bard_os_ref;
BardGlobalRef bard_os_version_ref;
BardGlobalRef bard_hw_version_ref;

ArrayList<BardNativeFn> bard_custom_shutdown_functions;


//=============================================================================
//  BardString
//=============================================================================
BardString* BardString::create( int count )
{
  BardString* string = (BardString*) 
    mm.create_object( BARD_TYPE_STRING, (sizeof(BardString)-2) + count*2 );
  string->count = count;
  return string;
}

BardString* BardString::create( BardChar* data, int count )
{
  BardString* string = create(count);
  memcpy( string->characters, data, count*2 );
  string->set_hash_code();
  return string;
};

BardString* BardString::create( const char* data, int count )
{
  if (count == -1) count = strlen(data);

  BardString* string = create(count);
  BardChar* characters = string->characters;
  for (int i=0; i<count; ++i)
  {
    characters[i] = data[i];
  }
  string->set_hash_code();
  return string;
};

void BardString::set_hash_code()
{
  BardChar* data = characters - 1;
  int c = count + 1;

  int code = 0;
  while (--c)
  {
    // left rotate by 7 bits 
    code = (code << 7) | ((code >> (16-7)) & ((1<<7) - 1));
    
    code += *(++data);
  }

  hash_code = code;
}

void BardString::to_ascii( char* buffer, int buffer_len )
{
  int c = buffer_len - 1;
  if (c > count) c = count;

  for (int i=0; i<c; ++i) buffer[i] = (char) characters[i];
  buffer[c] =  0;
}

char* BardString::to_new_ascii()
{
  char* result = new char[count+1];
  for (int i=0; i<count; ++i) result[i] = (char) characters[i];
  result[count] = 0;
  return result;
}

#if defined(BARD_XC)
  extern BardTypeInfo type_ClassWeakReference;
#endif

//=============================================================================
//  BardWeakRef
//=============================================================================
BardWeakRef* BardWeakRef::create( BardObject* object )
{
  BardWeakRef* ref = (BardWeakRef*) 
    mm.create_object( BARD_TYPE_WEAK_REFERENCE, (sizeof(BardWeakRef)) );
  ref->set( object );
  return ref;
}

BardWeakRef::BardWeakRef( BardObject* object ) : object(object)
{
  if (object) mm.weak_refs.add( this );
}

void BardWeakRef::set( BardObject* new_object )
{
  if (object)
  {
    if (new_object)
    {
      // No need to adjust weak ref list
      object = new_object;
    }
    else
    {
      object = NULL;
      mm.weak_refs.remove_value( this );
    }
  }
  else
  {
    if (new_object)
    {
      object = new_object;
      mm.weak_refs.add( this );
    }
  }
}

//=============================================================================
//  BardNativeData
//=============================================================================
void BardNativeDataNoDelete( void* data )
{
}

void BardNativeDataGenericDelete( void* data )
{
  delete (char*) data;
}

void BardNativeDataDeleteResource( void* data )
{
  delete ((BardResource*) data);
}

#if defined(BARD_VM)
BardNativeData* BardNativeData::create( void* data, BardNativeDataDeleteFn delete_fn )
{
  BardNativeData* result = (BardNativeData*) mm.create_object( 
      bvm.type_native_data, sizeof(BardNativeData) );
  result->init( data, delete_fn );
  return result;
}
#else
extern BardTypeInfo type_ClassNativeData;

BardNativeData* BardNativeData::create( void* data, BardNativeDataDeleteFn delete_fn )
{
  BardNativeData* result = (BardNativeData*) 
    mm.create_object( &type_ClassNativeData, sizeof(BardNativeData) );
  result->init( data, delete_fn );
  return result;
}
#endif

#ifdef _WIN32
  WSADATA winsock_data;
#endif

void bard_shut_down()
{
  for (int i=0; i<bard_custom_shutdown_functions.count; ++i)
  {
    bard_custom_shutdown_functions[i]();
  }
  bard_custom_shutdown_functions.clear();

#if defined(BARD_VM)
  bvm.shut_down();
#else
  sxc.shut_down();
#endif
}

void bard_init()
{
  mm.init();
#if defined(BARD_VM)
  bvm.init();
#else
  sxc.init();
#endif
}

void bard_configure()
{
#if defined(UNIX)
  signal( SIGPIPE, SIG_IGN );  // don't terminate program on a socket error
#elif _WIN32
  if (0 != WSAStartup( MAKEWORD(2,2), &winsock_data ))
  {
    bard_error_message.copy( "Unable to start WinSock." );
    throw 1;
  }
  /*
  else
  {
    if ( LOBYTE( winsock_data.wVersion ) != 2 ||
         HIBYTE( winsock_data.wVersion ) != 2 )
    {
      WSACleanup();
      bard_throw_fatal_error( "WinSock - invalid version." );
    }
  }
  */
#endif

#if defined(BARD_VM)
  bvm.configure();
#else
  sxc_configure();
#endif
}

void bard_launch()
{
  BARD_FIND_TYPE( type_SignalManager, "SignalManager" );
  BARD_PUSH_REF( type_SignalManager->singleton() );
  BARD_DUPLICATE_REF();

  BARD_PUSH_REF( BardString::create("launch") );
  BARD_PUSH_REF( bard_main_object );
  BARD_CALL( type_SignalManager, "queue_native(String,Object)" );

  bool repeat = true;
  while (repeat)
  {
    repeat = false;

    mm.check_gc();

    BARD_DUPLICATE_REF();
    BARD_CALL( type_SignalManager, "raise_pending()" );
    repeat = BARD_POP_LOGICAL();
  }

  BARD_DISCARD_REF();
}


void bard_register_native_method( const char* class_name, const char* method_name, BardNativeFn fn_ptr )
{
#if defined(BARD_VM)
  bvm.register_native_method( class_name, method_name, fn_ptr );
#endif
}

void bard_add_custom_shutdown( BardNativeFn fn )
{
  bard_custom_shutdown_functions.add( fn );
}

#if defined(BARD_USE_LONGJMP)
extern jmp_buf bard_fatal_jumppoint;
void bard_throw_fatal_error( const char* st )
{
  bard_error_message.copy( st );
  longjmp( bard_fatal_jumppoint, 1 );
}
#else
void bard_throw_fatal_error( const char* st )
{
  bard_error_message.copy( st );
  throw 1;
}
#endif

void bard_throw_fatal_error( const char* st1, const char* st2, const char* st3 )
{
  int len1 = strlen(st1);
  int len2 = strlen(st2);
  int len3 = strlen(st3);
  char* result = new char[len1+len2+len3+1];
  strcpy( result, st1 );
  strcat( result, st2 );
  strcat( result, st3 );
  bard_throw_fatal_error( result );
}

void bard_throw_fatal_error( const char* st1, const char* st2, const char* st3,
    const char* st4, const char* st5 )
{
  int len1 = strlen(st1);
  int len2 = strlen(st2);
  int len3 = strlen(st3);
  int len4 = strlen(st4);
  int len5 = strlen(st5);
  char* result = new char[len1+len2+len3+len4+len5+1];
  strcpy( result, st1 );
  strcat( result, st2 );
  strcat( result, st3 );
  strcat( result, st4 );
  strcat( result, st5 );
  bard_throw_fatal_error( result );
}

BardArray*     bard_create_byte_array( char* data, int count )
{
  BARD_FIND_TYPE( type_byte_array, "Array<<Byte>>" );
  BardArray* array = type_byte_array->create( count );
  if (data)
  {
    memcpy( array->data, data, count );
  }
  return array;
}

BardArrayList* bard_create_byte_list( char* data, int count )
{
  BARD_FIND_TYPE( type_byte_list, "ArrayList<<Byte>>" );
  BardLocalRef list = type_byte_list->create();

  BARD_FIND_TYPE( type_byte_array, "Array<<Byte>>" );
  BardArray* array = type_byte_array->create( count );
  if (data)
  {
    memcpy( array->data, data, count );
  }

  BARD_SET_REF( *list, "data", array );
  BARD_SET_INT32( *list, "count", count );

  return (BardArrayList*) *list;
}

BardArray*     bard_create_char_array( BardChar* data, int count )
{
  BARD_FIND_TYPE( type_char_array, "Array<<Char>>" );
  BardArray* array = type_char_array->create( count );
  if (data)
  {
    memcpy( array->data, data, count*2 );
  }
  return array;
}

BardArrayList* bard_create_char_list( BardChar* data, int count )
{
  BARD_FIND_TYPE( type_char_list, "ArrayList<<Char>>" );
  BardLocalRef list = type_char_list->create();

  BARD_FIND_TYPE( type_char_array, "Array<<Char>>" );
  BardArray* array = type_char_array->create( count );
  if (data)
  {
    memcpy( array->data, data, count );
  }

  BARD_SET_REF( *list, "data", array );
  BARD_SET_INT32( *list, "count", count );

  return (BardArrayList*) *list;
}

BardArray* bard_duplicate_array( BardObject* obj )
{
  if (obj == NULL) return NULL;

  BardLocalRef gc_guard(obj);

  BardArray* original = (BardArray*) obj;
  int count = original->array_count;

  BardArray* duplicate = original->type->create( count );

  memcpy( duplicate->data, original->data, count * original->type->element_size );

  return duplicate;
}

BardObject* bard_thrown_error = NULL;
#ifdef BARD_USE_LONGJMP
  jmp_buf bard_fatal_jumppoint;
  BardCatchHandler* bard_cur_catch = NULL;

  BardCatchHandler::BardCatchHandler()
  {
    prev = bard_cur_catch;
    bard_cur_catch = this;
  }

  BardCatchHandler::~BardCatchHandler()
  {
    unhook();
  }

  void BardCatchHandler::unhook()
  {
    if (bard_cur_catch == this) bard_cur_catch = prev;
  }
#endif

