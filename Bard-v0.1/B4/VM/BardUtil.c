#include <fcntl.h>
#include <math.h>
#include <strings.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#if !defined(_WIN32)
#  include <sys/time.h>
#  include <unistd.h>
#  include <signal.h>
#  include <dirent.h>
#  include <sys/socket.h>
#  include <sys/uio.h>
#  include <sys/stat.h>
#  include <netdb.h>
#  include <errno.h>
#  include <pthread.h>
#endif

#if defined(ANDROID)
#  include <netinet/in.h>
#endif

#if defined(_WIN32)
#  include <direct.h>
#  define chdir _chdir
#endif

#if TARGET_OS_IPHONE 
#  include <sys/types.h>
#  include <sys/sysctl.h>
#endif


#include "Bard.h"

//=============================================================================
//  Miscellaneous
//=============================================================================
int BardCString_hash_code( const char* st )
{
  int i;
  int hash_code = 0;
  for (i=0; st[i]; ++i)
  {
    hash_code = hash_code * 7 + st[i];
  }
  return hash_code;
}

int BardCString_find( const char* st, char ch )
{
  int i = -1;
  while (st[++i])
  {
    if (st[i] == ch) return i;
  }
  return -1;
}

char* BardCString_duplicate( const char* st )
{
  char* buffer;
  int   count;

  count = strlen( st );

  buffer = BARD_ALLOCATE( count + 1 );

  strcpy( buffer, st );

  return buffer;
}

//=============================================================================
//  BardPointerList
//=============================================================================
BardPointerList* BardPointerList_create()
{
  return BardPointerList_init( BARD_ALLOCATE( sizeof(BardPointerList) ) );
}

void BardPointerList_free( BardPointerList* list )
{
  if ( !list ) return;
  BardPointerList_release( list );
  BARD_FREE( list );
}

BardPointerList* BardPointerList_init( BardPointerList* list )
{
  bzero( list, sizeof(BardPointerList) );
  return list;
}

void BardPointerList_release( BardPointerList* list )
{
  if ( !list ) return;
  if (list->data)
  {
    BARD_FREE( list->data );
    list->data = NULL;
  }
  list->capacity = list->count = 0;
}

BardPointerList* BardPointerList_clear( BardPointerList* list )
{
  list->count = 0;
  return list;
}

BardPointerList* BardPointerList_free_elements( BardPointerList* list )
{
  while (list->count > 0)
  {
    BARD_FREE( list->data[ --list->count ] );
  }
  return list;
}

void BardPointerList_ensure_capacity( BardPointerList* list, int min_capacity )
{
  if (list->capacity >= min_capacity) return;

  if (list->data)
  {
    void** new_data = BARD_ALLOCATE( sizeof(void*) * min_capacity );
    memcpy( new_data, list->data, list->count * sizeof(void*) );
    BARD_FREE( list->data );
    list->data = new_data;
  }
  else
  {
    list->data = BARD_ALLOCATE( sizeof(void*) * min_capacity );
  }
  list->capacity = min_capacity;
}

void BardPointerList_add( BardPointerList* list, void* value )
{
  if (list->data)
  {
    if (list->count == list->capacity)
    {
      BardPointerList_ensure_capacity( list, list->capacity<<1 );
    }
  }
  else
  {
    BardPointerList_ensure_capacity( list, 10 );
  }

  list->data[ list->count++ ] = value;
}

//=============================================================================
//  BardUtil
//=============================================================================
BardReal BardUtil_time()
{
#if defined(_WIN32)
  struct __timeb64 time_struct;
  BardReal time_seconds;
  _ftime64_s( &time_struct );
  time_seconds = (BardReal) time_struct.time;
  time_seconds += time_struct.millitm / 1000.0;
  return time_seconds;
#elif defined(RVL)
  BardReal time_seconds = OSTicksToMilliseconds(OSGetTime()) / 1000.0;
  return time_seconds;

#else
  struct timeval time_struct;
  BardReal time_seconds;
  gettimeofday( &time_struct, 0 );
  time_seconds = (BardReal) time_struct.tv_sec;
  time_seconds += (time_struct.tv_usec / 1000000.0);
  return time_seconds;
#endif
}

