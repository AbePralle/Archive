#ifndef BARD_H
#define BARD_H
//=============================================================================
//  bard.h
//
//  General Bard Defines
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

#if defined(BARD_VM)
#elif defined(BARD_XC)
#else
#  error Either BARD_VM or BARD_XC must be defined.
#endif

#if defined(_WIN32)
#elif defined(RVL)
#else
  // Probably Mac or Unix... most commands work for both
#  define UNIX
#endif

#if defined(ANDROID) || TARGET_OS_IPHONE
#  define MOBILE_PLATFORM
#endif

#ifdef UNIX
#  include <limits.h>
#endif

#ifdef __APPLE__
#  include <TargetConditionals.h>
#  if TARGET_OS_IPHONE == 0
#    define MAC
#  endif
#endif

#define BARD_STACK_SIZE (8*1024)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#ifdef _WIN32
#  include <crtdbg.h>
#  include <winsock2.h>
#  include <Ws2tcpip.h>
#  include <windows.h>
#  define  PATH_MAX MAX_PATH
#  define  PATH_SEPARATOR ';'
#  define  DIR_SEPARATOR '\\'
   extern WSADATA winsock_data;
#else
#  include <sys/time.h>
#  include <unistd.h>
#  define  PATH_SEPARATOR ':'
#  define  DIR_SEPARATOR '/'
#endif

#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef BARD_USE_LONGJMP
# include <setjmp.h>
#endif

#if defined(_WIN32)
  typedef __int64          BardInt64;
  typedef __int32          BardInt32;
  typedef __int16          BardInt16;
  typedef signed char      BardInt8;
  typedef unsigned __int16 BardChar;
  typedef unsigned char    BardByte;
  typedef double           BardReal64;
  typedef float            BardReal32;
  typedef BardInt32        BardLogical;
#else
  typedef long long        BardInt64;
  typedef int              BardInt32;
  typedef short            BardInt16;
  typedef signed char      BardInt8;
  typedef unsigned short   BardChar;
  typedef unsigned char    BardByte;
  typedef double           BardReal64;
  typedef float            BardReal32;
  typedef BardInt32        BardLogical;
#endif

struct BardAlignmentInfo
{
  BardInt32  integer;
  BardReal64 real;
};


//=============================================================================
//  ArrayList
//
//  Arraylist for items that don't need to be deleted.
//=============================================================================
template <typename DataType> 
struct ArrayList
{
  DataType* data;
  int count;
  int capacity;

  ArrayList( int capacity=0 )
  {
    this->capacity = capacity;
    if (capacity)
    {
      data = new DataType[capacity];
      memset( data, 0, sizeof(DataType)*capacity );
    }
    else data = 0;
    count = 0;
  }

  ~ArrayList()
  {
    release();
  }

  void release()
  {
    if (data) 
    {
      delete data;
      data = 0;
      count = 0;
      capacity = 0;
    }
  }

  void clear() { count = 0; }

  void add( DataType value )
  {
    if (count == capacity) ensure_capacity( capacity ? capacity*2 : 10 );
    data[count++] = value;
  }

  DataType remove( int index )
  {
    if (count == 1 || index == count-1)
    {
      return data[--count];
    }
    else
    {
      DataType result = data[index];
      --count;
      while (index < count)
      {
        data[index] = data[index+1];
        ++index;
      }
      return result;
    }
  }

  void remove_value( DataType value )
  {
    for (int i=count-1; i>=0; --i)
    {
      if (data[i] == value)
      {
        remove(i);
        return;
      }
    }
  }

  DataType remove_last()
  {
    return data[--count];
  }

  inline DataType& operator[]( int index ) { return data[index]; }

  DataType first() { return data[0]; }
  DataType last() { return data[count-1]; }

  void ensure_capacity( int c )
  {
    if (capacity >= c) return;

    capacity = c;

    int bytes = sizeof(DataType) * capacity;

    if ( !data )
    {
      data = new DataType[capacity];
      memset( data, 0, sizeof(DataType) * capacity );
    }
    else
    {
      int old_bytes = sizeof(DataType) * count;

      DataType* new_data = new DataType[capacity];
      memset( ((char*)new_data) + old_bytes, 0, bytes - old_bytes );
      memcpy( new_data, data, old_bytes );

      delete data;
      data = new_data;
    }
  }
};

//=============================================================================
//  AllocList<DataType>
//  An arraylist whose elements are deleted on exit.
//=============================================================================
template <typename DataType> 
struct AllocList
{
  DataType* data;
  int count;
  int capacity;

  AllocList( int capacity=0 )
  {
    this->capacity = capacity;
    if (capacity)
    {
      data = new DataType[capacity];
      memset( data, 0, sizeof(DataType)*capacity );
    }
    else data = 0;
    count = 0;
  }

  ~AllocList()
  {
    clear();
    if (data) 
    {
      delete data;
      data = 0;
    }
  }

  void clear() 
  { 
    for (int i=0; i<count; ++i)
    {
      if (data[i])
      {
        delete data[i];
        data[i] = 0;
      }
    }
    count = 0; 
  }

  void add( DataType value )
  {
    if (count == capacity) ensure_capacity( capacity ? capacity*2 : 10 );
    data[count++] = value;
  }

  DataType remove_last()
  {
    return data[--count];
  }

  inline DataType& operator[]( int index ) { return data[index]; }

  DataType first() { return data[0]; }
  DataType last() { return data[count-1]; }

  bool contains( DataType value )
  {
    for (int i=0; i<count; ++i)
    {
      if (data[i] == value) return true;
    }
    return false;
  }

  void ensure_capacity( int c )
  {
    if (capacity >= c) return;

    capacity = c;

    int bytes = sizeof(DataType) * capacity;

    if ( !data )
    {
      data = new DataType[capacity];
      memset( data, 0, sizeof(DataType) * capacity );
    }
    else
    {
      int old_bytes = sizeof(DataType) * count;

      DataType* new_data = new DataType[capacity];
      memset( ((char*)new_data) + old_bytes, 0, bytes - old_bytes );
      memcpy( new_data, data, old_bytes );

      delete data;
      data = new_data;
    }
  }
};

//=============================================================================
//  ASCIIString
//  Simple auto-delete string.
//=============================================================================
struct ASCIIString
{
  char* value;

  ASCIIString() : value(NULL) { }

  ASCIIString( const char *value ) : value((char*)value)
  {
  }

  ~ASCIIString()
  {
    if (value)
    {
      delete value;
      value = NULL;
    }
  }

  void copy( const char* st )
  {
    if (value) delete value;
    int len = strlen( st );
    value = new char[ len + 1 ];
    strcpy( value, st );
  }

  void operator=( const char* value )
  {
    if (this->value) delete this->value;
    this->value = (char*)value;
  }
};


#include "bard_runtime.h"

#include "bard_mm.h"
#if defined(BARD_VM)
#  include "bard_vm.h"
#else
#  include "bard_xc.h"
#endif
#include "bard_stdlib.h"

#endif // BARD_H

