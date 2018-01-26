//=============================================================================
//  BossAllocator.cpp
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"

namespace Boss
{

//-----------------------------------------------------------------------------
//  AllocationPage
//-----------------------------------------------------------------------------
AllocationPage::AllocationPage( AllocationPage* next_page )
  : next_page(next_page)
{
  cursor = data;
  remaining = BOSSMM_PAGE_SIZE;
  //printf( "New page\n");
}

void* AllocationPage::allocate( int size )
{
  // Round size up to multiple of 8.
  if (size > 0) size = (size + 7) & ~7;
  else          size = 8;

  if (size > remaining) return NULL;

  //printf( "Allocating %d bytes from page.\n", size );
  void* result = cursor;
  cursor += size;
  remaining -= size;

  //printf( "%d / %d\n", BOSSMM_PAGE_SIZE - remaining, BOSSMM_PAGE_SIZE );
  return result;
}


//-----------------------------------------------------------------------------
//  Allocator
//-----------------------------------------------------------------------------
Allocator::Allocator() : pages(NULL)
{
  for (int i=0; i<BOSSMM_SLOT_COUNT; ++i)
  {
    free_objects[i] = NULL;
  }
}

Allocator::~Allocator()
{
  while (pages)
  {
    AllocationPage* next_page = pages->next_page;
    delete pages;
    pages = next_page;
  }
}

void* Allocator::allocate( int size )
{
  if (size > BOSSMM_SMALL_ALLOCATION_SIZE_LIMIT) return malloc( size );

  if (size <= 0) size = BOSSMM_GRANULARITY_SIZE;
  else           size = (size + BOSSMM_GRANULARITY_MASK) & ~BOSSMM_GRANULARITY_MASK;

  int slot = (size >> BOSSMM_GRANULARITY_BITS);
  Allocation* obj = free_objects[slot];
  
  if (obj)
  {
    //printf( "found free object\n");
    free_objects[slot] = obj->next_allocation;
    return obj;
  }

  // No free objects for requested size.

  // Try allocating a new object from the current page.
  if ( !pages )
  {
    pages = new AllocationPage(NULL);
  }

  obj = (Allocation*) pages->allocate( size );
  if (obj) return obj;


  // Not enough room on allocation page.  Allocate any smaller blocks
  // we're able to and then move on to a new page.
  int s = slot - 1;
  while (s >= 1)
  {
    obj = (Allocation*) pages->allocate( s << BOSSMM_GRANULARITY_BITS );
    if (obj)
    {
      //printf( "free obj size %d\n", (s << BOSSMM_GRANULARITY_BITS) );
      obj->next_allocation = free_objects[s];
      free_objects[s] = obj;
    }
    else
    {
      --s;
    }
  }

  // New page; this will work for sure.
  pages = new AllocationPage( pages );
  return pages->allocate( size );
}


void* Allocator::free( void* data, int size )
{
  if (data)
  {
    if (size > BOSSMM_SMALL_ALLOCATION_SIZE_LIMIT)
    {
      ::free( data );
    }
    else
    {
      // Return object to small allocation pool
      Allocation* allocation = (Allocation*) data;
      int slot = (size + BOSSMM_GRANULARITY_MASK) >> BOSSMM_GRANULARITY_BITS;
      if (slot <= 0) slot = 1;
      allocation->next_allocation = free_objects[slot];
      free_objects[slot] = allocation;
    }
  }

  // Always returns null, allowing a pointer to be freed and assigned null in
  // a single step.
  return NULL;
}

}; // namespace Boss
