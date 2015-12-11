#include <stdlib.h>
#include <strings.h>

#include "BardDefines.h"
// Only used for definitions of BARD_ALLOCATE and BARD_FREE

#include "BardAllocator.h"

#ifndef BARD_ALLOCATOR_PAGE_SIZE
#  define BARD_ALLOCATOR_PAGE_SIZE (128*1024)
#endif

enum
{
  BARD_ALLOCATOR_SLOT_COUNT               =  17,
    // - The slot index times 16 is the allocation size of that slot.
    // - Last slot is unlimited size (regular allocation)
    // - Not all slots are used - allocator restricts values to maximize block
    //   reuse for varying sizes.  Only sizes 16, 32, 48, 64, (small allocations)
    //   and 128, 192, 256 (medium allocations) are used.
  BARD_ALLOCATOR_SMALL_OBJECT_BITS        =   4,
  BARD_ALLOCATOR_SMALL_OBJECT_SIZE_LIMIT  = ((BARD_ALLOCATOR_SLOT_COUNT-1) << BARD_ALLOCATOR_SMALL_OBJECT_BITS)
};



//=============================================================================
// GLOBALS
//=============================================================================
static int BardAllocator_byte_size_to_slot_size[] =
{
  // Size 0, 1..16  -> 16 bytes
  16, 
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,

  // Size 17..32 -> 32 bytes
  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,

  // Size 33..48 -> 48 bytes
  48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,

  // Size 49..63 -> 64 bytes
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,

  // Size 65..128 -> 128 bytes
  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,

  // Size 129..192 -> 192 bytes
  192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
  192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
  192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
  192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,

  // Size 193..256 -> 256 bytes
  256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
  256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
  256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
  256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
};


//=============================================================================
//  BardMMAllocation
//=============================================================================
typedef struct BardMMAllocation
{
  struct BardMMAllocation* next_allocation;
  struct BardMMAllocation* next_reference;
} BardMMAllocation;


//=============================================================================
// BardMMAllocationPage
//=============================================================================
typedef struct BardMMAllocationPage
{
  struct BardMMAllocationPage* next_page;
  unsigned char*               data;
} BardMMAllocationPage;

BardMMAllocationPage* BardMMAllocationPage_create( int bytes, BardMMAllocationPage* next_page )
{
  BardMMAllocationPage* page = BARD_ALLOCATE( sizeof(BardMMAllocationPage) );

  page->data = BARD_ALLOCATE( bytes );
  bzero( page->data, bytes );

  page->next_page = next_page;

  return page;
}

void BardMMAllocationPage_release( BardMMAllocationPage* page )
{
  if ( !page ) return;

  BardMMAllocationPage_release( page->next_page );
  page->next_page = NULL;

  BARD_FREE( page->data );
  page->data = NULL;
}

void BardMMAllocationPage_free( BardMMAllocationPage* page )
{
  if ( !page ) return;
  BardMMAllocationPage_release( page );
  BARD_FREE( page );
}


//=============================================================================
// BardAllocator
//=============================================================================
typedef struct BardAllocator
{
  BardMMAllocationPage* permanent_allocation_pages;
  unsigned char*        permanent_allocation_cursor;
  int                   permanent_allocation_bytes_remaining;
  int                   initialized;

  BardMMAllocation*     small_object_pools[ BARD_ALLOCATOR_SLOT_COUNT ];
    // Size  unlimited,       // [0]
    //       16, 32, 48, 64,  // [1..4]
    //       x,  x,  x,128,   // [5..8]
    //       x,  x,  x,192,   // [9..12]
    //       x,  x,  x,256    // [13..16]

} BardAllocator;

// A globally accessible allocator that is used by all BardMM managers to
// recycle small object allocations.
static BardAllocator BardAllocator_singleton;  // static storage initialized to 0/NULL

void  BardAllocator_initialize();
void  BardAllocator_reset();

void BardAllocator_initialize()
{
  if (BardAllocator_singleton.initialized) return;

  BardAllocator_reset();

  bzero( &BardAllocator_singleton, sizeof(BardAllocator) );

  BardAllocator_singleton.permanent_allocation_pages  = BardMMAllocationPage_create( BARD_ALLOCATOR_PAGE_SIZE, NULL );
  BardAllocator_singleton.permanent_allocation_cursor = BardAllocator_singleton.permanent_allocation_pages->data;
  BardAllocator_singleton.permanent_allocation_bytes_remaining = BARD_ALLOCATOR_PAGE_SIZE;

  BardAllocator_singleton.initialized = 1;
}

void BardAllocator_reset()
{
  if ( !BardAllocator_singleton.initialized ) return;
  BardAllocator_singleton.initialized = 0;

  BardMMAllocationPage_free( BardAllocator_singleton.permanent_allocation_pages );
  BardAllocator_singleton.permanent_allocation_pages = NULL;
}

//int total_allocations = 0;
void* BardAllocator_allocate_permanent( int bytes )
{
  // Returns a block of memory that will only be freed when the program terminates.
  // Benefits: very fast allocations from larger pages of memory.

  // align to 8-byte boundary
  bytes = (bytes + 7) & ~7;
  if ( !bytes ) bytes = 8;

  if (bytes > BardAllocator_singleton.permanent_allocation_bytes_remaining)
  {
    if (bytes > BARD_ALLOCATOR_PAGE_SIZE || BardAllocator_singleton.permanent_allocation_bytes_remaining > 1024)
    {
      // Either it's a big allocation or it's manageable but we would be wasting too much
      // memory to start a new page now.  Make a special-sized page but don't reset the 
      // allocation cursor.
      BardAllocator_singleton.permanent_allocation_pages = BardMMAllocationPage_create( bytes, BardAllocator_singleton.permanent_allocation_pages );
      return BardAllocator_singleton.permanent_allocation_pages->data;
    }
    else
    {
      // Ran out of room; start a new allocation page.

      // First put all remaining space into small object caches to avoid wasting memory
      while (BardAllocator_singleton.permanent_allocation_bytes_remaining >= 128)
      {
        BardAllocator_free( BardAllocator_allocate_permanent(128), 128 );
      }
      while (BardAllocator_singleton.permanent_allocation_bytes_remaining >= 64)
      {
        BardAllocator_free( BardAllocator_allocate_permanent(64), 64 );
      }
      while (BardAllocator_singleton.permanent_allocation_bytes_remaining >= 48)
      {
        BardAllocator_free( BardAllocator_allocate_permanent(48), 48 );
      }
      while (BardAllocator_singleton.permanent_allocation_bytes_remaining >= 32)
      {
        BardAllocator_free( BardAllocator_allocate_permanent(32), 32 );
      }


      BardAllocator_singleton.permanent_allocation_pages = BardMMAllocationPage_create( 
          BARD_ALLOCATOR_PAGE_SIZE, BardAllocator_singleton.permanent_allocation_pages );
      BardAllocator_singleton.permanent_allocation_cursor = BardAllocator_singleton.permanent_allocation_pages->data;
      BardAllocator_singleton.permanent_allocation_bytes_remaining = BARD_ALLOCATOR_PAGE_SIZE;
    }
  }

  BardMMAllocation* result = (BardMMAllocation*) BardAllocator_singleton.permanent_allocation_cursor;
  BardAllocator_singleton.permanent_allocation_cursor += bytes;
  BardAllocator_singleton.permanent_allocation_bytes_remaining -= bytes;

  // Note: permanent allocation pages are zero-filled when first created.
  return result;
}

void* BardAllocator_allocate( int bytes )
{
  // INTERNAL USE
  // 
  // Contract
  // - The calling function is responsible for tracking the returned memory 
  //   and returning it via BardAllocator_singleton.free( void*, int )
  // - The returned memory is initialized to zero.
  if (bytes <= BARD_ALLOCATOR_SMALL_OBJECT_SIZE_LIMIT)
  {
    bytes = BardAllocator_byte_size_to_slot_size[ bytes ];
    int pool_index = bytes >> BARD_ALLOCATOR_SMALL_OBJECT_BITS;

    BardMMAllocation* result = BardAllocator_singleton.small_object_pools[ pool_index ];
    if (result)
    {
      BardAllocator_singleton.small_object_pools[pool_index] = result->next_allocation;
      bzero( result, bytes );
      return result;
    }
    else
    {
      return BardAllocator_allocate_permanent( bytes );
    }
  }
  else
  {
    void* result = BARD_ALLOCATE( bytes );
    bzero( result, bytes );
    return result;
  }
}

void BardAllocator_free( void* ptr, int bytes )
{
#ifdef BARD_DEBUG
  memset( ptr, -1, bytes );
#endif
  if (bytes <= BARD_ALLOCATOR_SMALL_OBJECT_SIZE_LIMIT)
  {
    bytes = BardAllocator_byte_size_to_slot_size[ bytes ];
    int pool_index = bytes >> BARD_ALLOCATOR_SMALL_OBJECT_BITS;

    BardMMAllocation* obj = (BardMMAllocation*) ptr;
    obj->next_allocation = BardAllocator_singleton.small_object_pools[pool_index];
    BardAllocator_singleton.small_object_pools[pool_index] = obj;
  }
  else
  {
    BARD_FREE( (char*) ptr );
  }
}

