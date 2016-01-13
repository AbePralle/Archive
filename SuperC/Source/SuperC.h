#ifndef SUPER_C_H
#define SUPER_C_H
//=============================================================================
//  SuperC.h
//
//  v1.0.2 - 2016.01.01 by Abe Pralle
//
//  See README.md for instructions.
//=============================================================================

#ifdef __cplusplus
  #define BEGIN_EXTERN_C extern "C" {
  #define END_EXTERN_C   }
#else
  #define BEGIN_EXTERN_C 
  #define END_EXTERN_C
#endif

BEGIN_EXTERN_C

#ifndef _CRT_SECURE_NO_WARNINGS
  #define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef C_MALLOC
#  define C_MALLOC(bytes) malloc(bytes)
#endif

#ifndef C_FREE
#  define C_FREE(ptr) free(ptr)
#endif

#ifdef _WIN64
  #define C_PLATFORM_WINDOWS         1
  #define C_PLATFORM_WINDOWS_64      1
#elif _WIN32
  #define C_PLATFORM_WINDOWS         1
  #define C_PLATFORM_WINDOWS_32      1
#elif __APPLE__
  #include "TargetConditionals.h"
  #if TARGET_IPHONE_SIMULATOR
    #define C_PLATFORM_IOS           1
    #define C_PLATFORM_IOS_SIMULATOR 1
  #elif TARGET_OS_IPHONE
    #define C_PLATFORM_IOS           1
    #define C_PLATFORM_IOS_DEVICE    1
  #else
    //   TARGET_OS_MAC
    #define C_PLATFORM_MAC           1
  #endif
#elif defined(ANDROID)
  #define C_PLATFORM_ANDROID         1
#elif defined(__linux__)
  #define C_PLATFORM_LINUX           1
  #define C_PLATFORM_UNIX            1
#elif defined(__unix__)
  #define C_PLATFORM_UNIX            1
#elif defined(_POSIX_VERSION)
  #define C_PLATFORM_POSIX           1
  #define C_PLATFORM_UNIX            1
#endif

//=============================================================================
//  Type Defs
//=============================================================================
#if defined(_WIN32)
  typedef double  CReal64;
  typedef float   CReal32;
  typedef __int64 CInt64;
  typedef __int32 CInt32;
  typedef __int16 CInt16;
  typedef __int8  CInt8;
  typedef unsigned __int64 CUInt64;
  typedef unsigned __int32 CUInt32;
  typedef unsigned __int16 CUInt16, CCharacter;
  typedef unsigned __int8  CUInt8, CByte;
  typedef int              CLogical;
#else
  typedef double    CReal64;
  typedef float     CReal32;
  typedef long long CInt64;
  typedef int       CInt32;
  typedef short     CInt16;
  typedef char      CInt8;
  typedef unsigned long long CUInt64;
  typedef unsigned int       CUInt32;
  typedef unsigned short     CUInt16, CCharacter;
  typedef unsigned char      CUInt8, CByte;
  typedef int       CLogical;
#endif

typedef CReal64 CReal;
typedef CReal32 CFloat;
typedef CInt64  CLong;
typedef CInt32  CInteger;


//=============================================================================
//  CIntList
//=============================================================================
typedef struct CIntList
{
  int*   data;
  int    count;
  int    capacity;
} CIntList;

CIntList* CIntList_create();
CIntList* CIntList_destroy( CIntList* list );

CIntList* CIntList_init( CIntList* list );
void      CIntList_retire( CIntList* list );

CIntList* CIntList_clear( CIntList* list );

void CIntList_add( CIntList* list, int value );
void CIntList_insert( CIntList* list, int value, int before_index );
int  CIntList_locate( CIntList* list, int value );
int  CIntList_remove_at( CIntList* list, int index );
int  CIntList_remove_last( CIntList* list );
void CIntList_reserve( CIntList* list, int additional_capacity );
void CIntList_set( CIntList* list, int index, int value );


//=============================================================================
//  CPointerList
//=============================================================================
typedef struct CPointerList
{
  void** data;
  int    count;
  int    capacity;
} CPointerList;

CPointerList* CPointerList_create();
CPointerList* CPointerList_destroy( CPointerList* list );

CPointerList* CPointerList_init( CPointerList* list );
void          CPointerList_retire( CPointerList* list );

CPointerList* CPointerList_clear( CPointerList* list );
CPointerList* CPointerList_free_elements( CPointerList* list );

void  CPointerList_add( CPointerList* list, void* value );
void  CPointerList_insert( CPointerList* list, void* value, int before_index );
int   CPointerList_locate( CPointerList* list, void* value );
void* CPointerList_remove_at( CPointerList* list, int index );
void* CPointerList_remove_last( CPointerList* list );
void  CPointerList_reserve( CPointerList* list, int additional_capacity );
void  CPointerList_set( CPointerList* list, int index, void* value );


//=============================================================================
//  CResourceBank 
//=============================================================================
typedef struct CResourceBank
{
  CPointerList*  resources;
  CIntList*      active_ids;
  CIntList*      available_ids;
} CResourceBank;

CResourceBank* CResourceBank_create();
CResourceBank* CResourceBank_destroy( CResourceBank* bank );

void  CResourceBank_init( CResourceBank* bank );
void  CResourceBank_retire( CResourceBank* bank );

int   CResourceBank_add( CResourceBank* bank, void* resource );
void  CResourceBank_clear( CResourceBank* bank );
int   CResourceBank_count( CResourceBank* bank );
int   CResourceBank_create_id( CResourceBank* bank );
int   CResourceBank_locate_first( CResourceBank* bank );
int   CResourceBank_locate_resource( CResourceBank* bank, void* resource );
void* CResourceBank_get( CResourceBank* bank, int id );
void* CResourceBank_remove( CResourceBank* bank, int id );
void* CResourceBank_remove_another( CResourceBank* bank );
void  CResourceBank_set( CResourceBank* bank, int id, void* resource );


//=============================================================================
//  Utility 
//=============================================================================

#define C_allocate(type)             ((type*)C_allocate_bytes_and_clear(sizeof(type)))
#define C_allocate_array(type,count) ((type*)C_allocate_bytes_and_clear(count*sizeof(type)))
void*   C_allocate_bytes_and_clear( int size ); // Supports convenience macro C_allocate(type)
void*   C_free( void* ptr );

double  C_current_time_seconds();

END_EXTERN_C

#endif // SUPER_C_H
