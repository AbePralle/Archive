#ifndef BARD_OBJECT_H
#define BARD_OBJECT_H

#include "Bard.h"

struct BardType;
struct BardVM;
struct BardArray;

//=============================================================================
//  BardObject
//=============================================================================
typedef struct BardObject
{
  struct BardObject* next_allocation;
  struct BardObject* next_reference;
  // This must come first - it echoes the next_allocation field in BardAllocator,
  // which is used there to chain free small allocations.

  struct BardType*   type;

  BardInteger        size;
    // - In bytes; complemented per-object during GC
    // - Size is >= 0 in-between GC's.
    // - During GC, size is set to ~size before tracing through.
    // - After trace, size<0 objects are collected (with size=~size) and
    //   size >= 0 objects are recycled or freed.

} BardObject;

#endif // BARD_OBJECT_H

