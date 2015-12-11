#ifndef BARD_MM_H
#define BARD_MM_H

//=============================================================================
// BardMM
//
// Invariant: 'size' value is 0+ for all objects between GCs.  During a GC
//            the size may become negative during the tracing process.
//
// New allocations go to new_objects or new_objects_requiring_cleanup.
//
// GC:
// 1) All referenced objects are traced through starting at the singletons.
// 2) forEach obj in transfer:objects
//    - referenced:   un-invert size and add to objects.
//    - unreferenced: release if unreferenced
// 3) forEach obj in transfer:objects_requiring_cleanup
//    - referenced:   un-invert size and add to objects_requiring_cleanup
//    - unreferenced: call clean_up() (may create new objects on objects[] 
//      and/or objects_requiring_cleanup lists) and add to objects.
// After trace, surviving new_objects
//=============================================================================

#include "Bard.h"

struct BardType;
struct BardVM;
//struct BardGlobalRef;

//=============================================================================
//  BardMM (per-VM)
//=============================================================================
typedef struct BardMM
{
  struct BardVM*        vm;
  struct BardObject*    objects;
  struct BardObject*    objects_requiring_cleanup;
  //struct BardGlobalRef* global_refs;

  int gc_requested;
  int bytes_allocated_since_gc;
} BardMM;

BardMM* BardMM_init( BardMM* mm, struct BardVM* vm );
void    BardMM_release( BardMM* mm );

int     BardMM_should_collect_garbage( BardMM* mm );

void BardMM_collect_garbage( BardMM* mm );
void BardMM_trace_references( BardMM* mm );
void BardMM_trace( struct BardObject* obj );

void BardMM_clean_all( BardMM* mm );
void BardMM_release_all( struct BardObject** list );

// TODO: C impl of these
void BardMM_clean_unused_objects( BardMM* mm );
void BardMM_release_unused_objects( BardMM* mm );

BardObject* BardMM_create_object( BardMM* mm, BardType* type, int size );

#endif  // BARD_MM_H

