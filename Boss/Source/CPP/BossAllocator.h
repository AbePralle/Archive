//=============================================================================
//  BossAllocator.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

//-----------------------------------------------------------------------------
//  Allocation
//-----------------------------------------------------------------------------
struct Allocation
{
  // PROPERTIES
  Integer     size;
  Integer     reference_count;
  Allocation* next_allocation;

  // METHODS
  Allocation* retain() { ++reference_count; return this; }
  Allocation* release() { --reference_count; return 0; }
};

//-----------------------------------------------------------------------------
//  AllocationPage
//-----------------------------------------------------------------------------
struct AllocationPage
{
  // Backs small 0..256-byte allocations.
  AllocationPage* next_page;

  Byte* cursor;
  int        remaining;

  Byte  data[ BOSSMM_PAGE_SIZE ];

  AllocationPage( AllocationPage* next_page );

  void* allocate( int size );
};

//-----------------------------------------------------------------------------
//  Allocator
//-----------------------------------------------------------------------------
struct Allocator
{
  AllocationPage* pages;
  Allocation*     free_objects[BOSSMM_SLOT_COUNT];

  Allocator();
  ~Allocator();
  void* allocate( int size );
  void* free( void* data, int size );
};

}; // namespace Boss
