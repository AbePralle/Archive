//=============================================================================
//  SuperC.c
//
//  v1.0.2 - 2016.01.01 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================

#include "SuperC.h"

#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>

#if !defined(C_PLATFORM_WINDOWS)
  #include <sys/time.h>
  #include <unistd.h>
  #include <signal.h>
  #include <dirent.h>
  #include <sys/socket.h>
  #include <sys/uio.h>
  #include <sys/stat.h>
  #include <netdb.h>
  #include <errno.h>
  #include <pthread.h>
#endif

#if defined(C_PLATFORM_ANDROID)
  #include <netinet/in.h>
#endif

#if defined(C_PLATFORM_WINDOWS)
  #include <direct.h>
  #define chdir _chdir
#endif

#if C_PLATFORM_IOS 
  #include <sys/types.h>
  #include <sys/sysctl.h>
#endif

BEGIN_EXTERN_C

//=============================================================================
//  CIntList
//=============================================================================
CIntList* CIntList_create()
{
  return CIntList_init( C_allocate(CIntList) );
}

CIntList* CIntList_destroy( CIntList* list )
{
  CIntList_retire( list );
  return (CIntList*) C_free( list );
}

CIntList* CIntList_init( CIntList* list )
{
  memset( list, 0, sizeof(CIntList) );
  return list;
}

void CIntList_retire( CIntList* list )
{
  if ( !list ) return;
  list->data = (int*) C_free( list->data );
  list->capacity = list->count = 0;
}

CIntList* CIntList_clear( CIntList* list )
{
  list->count = 0;
  return list;
}

void CIntList_add( CIntList* list, int value )
{
  CIntList_reserve( list, 1 );
  list->data[ list->count++ ] = value;
}

void CIntList_insert( CIntList* list, int value, int before_index )
{
  if (before_index >= list->count)
  {
    CIntList_add( list, value );
    return;
  }

  if (before_index < 0) before_index = 0;

  CIntList_reserve( list, 1 );

  memmove( 
      list->data + before_index + 1,
      list->data + before_index,
      (list->count - before_index) * sizeof(int)
    );
  list->data[ before_index ] = value;
  ++list->count;
}

int CIntList_locate( CIntList* list, int value )
{
  int  i;
  int  count = list->count;
  int* data = list->data;

  for (i=0; i<count; ++i)
  {
    if (data[i] == value) return i;
  }

  return -1;
}

int CIntList_remove_at( CIntList* list, int index )
{
  if (index == list->count - 1)
  {
    return CIntList_remove_last( list );
  }
  else
  {
    int result = list->data[ index ];
    memmove( list->data + index, list->data + index + 1, (--list->count - index) * sizeof(int) );
    return result;
  }
}

int CIntList_remove_last( CIntList* list )
{
  return list->data[ --list->count ];
}

void CIntList_reserve( CIntList* list, int additional_capacity )
{
  int required_capacity = list->count + additional_capacity;
  if (list->capacity >= required_capacity) return;

  if (list->data)
  {
    int double_capacity = list->count << 1;
    if (double_capacity > required_capacity) required_capacity = double_capacity;

    int* new_data = C_allocate_array( int, required_capacity );
    memcpy( new_data, list->data, list->count * sizeof(int) );
    C_free( list->data );
    list->data = new_data;
  }
  else
  {
    if (required_capacity < 10) required_capacity = 10;
    list->data = C_allocate_array( int, required_capacity );
  }

  list->capacity = required_capacity;
}

void CIntList_set( CIntList* list, int index, int value )
{
  CIntList_reserve( list, (index+1) - list->count );
  if (list->count <= index) list->count = index + 1;

  list->data[ index ] = value;
}


//=============================================================================
//  CPointerList
//=============================================================================
CPointerList* CPointerList_create()
{
  return CPointerList_init( C_allocate( CPointerList ) );
}

CPointerList* CPointerList_destroy( CPointerList* list )
{
  CPointerList_retire( list );
  return (CPointerList*) C_free( list );
}

CPointerList* CPointerList_init( CPointerList* list )
{
  memset( list, 0, sizeof(CPointerList) );
  return list;
}

void CPointerList_retire( CPointerList* list )
{
  if ( !list ) return;
  list->data = (void**) C_free( list->data );
  list->capacity = list->count = 0;
}

CPointerList* CPointerList_clear( CPointerList* list )
{
  list->count = 0;
  return list;
}

CPointerList* CPointerList_free_elements( CPointerList* list )
{
  while (list->count > 0)
  {
    C_free( list->data[ --list->count ] );
  }
  return list;
}

void CPointerList_add( CPointerList* list, void* value )
{
  CPointerList_reserve( list, 1 );
  list->data[ list->count++ ] = value;
}

void CPointerList_insert( CPointerList* list, void* value, int before_index )
{
  if (before_index >= list->count)
  {
    CPointerList_add( list, value );
    return;
  }

  if (before_index < 0) before_index = 0;

  CPointerList_reserve( list, 1 );

  memmove( 
      list->data + before_index + 1,
      list->data + before_index,
      (list->count - before_index) * sizeof(void*)
    );
  list->data[ before_index ] = value;
  ++list->count;
}


int CPointerList_locate( CPointerList* list, void* value )
{
  int    i;
  int    count = list->count;
  void** data = list->data;

  for (i=0; i<count; ++i)
  {
    if (data[i] == value) return i;
  }

  return -1;
}

void* CPointerList_remove_at( CPointerList* list, int index )
{
  if (index == list->count - 1)
  {
    return CPointerList_remove_last( list );
  }
  else
  {
    void* result = list->data[ index ];
    memmove( list->data + index, list->data + index + 1, (--list->count - index) * sizeof(void*) );
    return result;
  }
}

void* CPointerList_remove_last( CPointerList* list )
{
  return list->data[ --list->count ];
}

void CPointerList_reserve( CPointerList* list, int additional_capacity )
{
  int required_capacity = list->count + additional_capacity;
  if (list->capacity >= required_capacity) return;

  if (list->data)
  {
    int double_capacity = list->count << 1;
    if (double_capacity > required_capacity) required_capacity = double_capacity;

    void** new_data = C_allocate_array( void*, required_capacity );
    memcpy( new_data, list->data, list->count * sizeof(void*) );
    C_free( list->data );
    list->data = new_data;
  }
  else
  {
    if (required_capacity < 10) required_capacity = 10;
    list->data = C_allocate_array( void*, required_capacity );
  }

  list->capacity = required_capacity;
}

void CPointerList_set( CPointerList* list, int index, void* value )
{
  CPointerList_reserve( list, (index+1) - list->count );
  if (list->count <= index) list->count = index + 1;

  list->data[ index ] = value;
}


//=============================================================================
//  CResourceBank
//=============================================================================
CResourceBank* CResourceBank_create()
{
  CResourceBank* bank = C_allocate( CResourceBank );
  CResourceBank_init( bank );
  return bank;
}

CResourceBank* CResourceBank_destroy( CResourceBank* bank )
{
  return (CResourceBank*) C_free( bank );
}

void CResourceBank_init( CResourceBank* bank )
{
  bank->resources = CPointerList_create();
  bank->active_ids = CIntList_create();
  bank->available_ids = CIntList_create();

  CPointerList_add( bank->resources, 0 );  // skip spot 0
}

void CResourceBank_retire( CResourceBank* bank )
{
  bank->resources = CPointerList_destroy( bank->resources );

  bank->active_ids = CIntList_destroy( bank->active_ids );
  bank->available_ids = CIntList_destroy( bank->available_ids );
}

int CResourceBank_add( CResourceBank* bank, void* resource )
{
  int index = CResourceBank_create_id( bank );
  CResourceBank_set( bank, index, resource );
  return index;
}

void CResourceBank_clear( CResourceBank* bank )
{
  CPointerList_clear( bank->resources );
  CIntList_clear( bank->active_ids );
  CIntList_clear( bank->available_ids );

  CPointerList_add( bank->resources, 0 );  // skip spot 0
}

int CResourceBank_count( CResourceBank* bank )
{
  return bank->active_ids->count;
}

int CResourceBank_create_id( CResourceBank* bank )
{
  // INTERNAL USE
  int result;
  if (bank->available_ids->count)
  {
    result = CIntList_remove_last( bank->available_ids );
  }
  else
  {
    result = bank->resources->count;
    CPointerList_add( bank->resources, 0 );
  }
  CIntList_add( bank->active_ids, result );
  return result;
}

int CResourceBank_locate_first( CResourceBank* bank )
{
  if (bank->active_ids->count == 0) return 0;
  return bank->active_ids->data[0];
}

int CResourceBank_locate_resource( CResourceBank* bank, void* resource )
{
  int    i = bank->active_ids->count;
  void** data = bank->resources->data;

  while (--i >= 0)
  {
    if (data[i] == resource) return i;
  }

  return -1;
}

void* CResourceBank_get( CResourceBank* bank, int id )
{
  if (id <= 0 || id >= bank->resources->count) return 0;
  return bank->resources->data[ id ];
}

void* CResourceBank_remove( CResourceBank* bank, int id )
{
  int   index;
  void* result;

  if (id <= 0 || id >= bank->resources->count) return 0;

  CIntList_add( bank->available_ids, id );

  result = bank->resources->data[ id ];
  bank->resources->data[ id ] = 0;

  index = CIntList_locate( bank->active_ids, id );
  if (index >= 0)
  {
    bank->active_ids->data[index] = bank->active_ids->data[ --bank->active_ids->count ];
  }

  return result;
}

void* CResourceBank_remove_another( CResourceBank* bank )
{
  if (bank->active_ids->count == 0) return 0;

  {
    int id = bank->active_ids->data[0];
    if ( !id ) return 0;

    return CResourceBank_remove( bank, id );
  }
}

void CResourceBank_set( CResourceBank* bank, int id, void* resource )
{
  if (id <= 0 || id >= bank->resources->count) return;
  bank->resources->data[ id ] = resource;
}


//=============================================================================
//  Utility 
//=============================================================================
void* C_allocate_bytes_and_clear( int size )
{
  void* result = C_MALLOC( size );
  memset( result, 0, size );
  return result;
}

void* C_free( void* ptr )
{
  if (ptr) C_FREE( ptr );
  return 0;
}

double  C_current_time_seconds()
{
#if defined(_WIN32)
  struct __timeb64 time_struct;
  double time_seconds;
  _ftime64_s( &time_struct );
  time_seconds = (double) time_struct.time;
  time_seconds += time_struct.millitm / 1000.0;
  return time_seconds;
#elif defined(RVL)
  double time_seconds = OSTicksToMilliseconds(OSGetTime()) / 1000.0;
  return time_seconds;

#else
  struct timeval time_struct;
  double time_seconds;
  gettimeofday( &time_struct, 0 );
  time_seconds = (double) time_struct.tv_sec;
  time_seconds += (time_struct.tv_usec / 1000000.0);
  return time_seconds;
#endif
}

END_EXTERN_C
