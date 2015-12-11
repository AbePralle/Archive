#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "Bard.h"

//=============================================================================
// BardMM
//=============================================================================
BardMM* BardMM_init( BardMM* mm, BardVM* vm )
{
  bzero( mm, sizeof(BardMM) );
  mm->vm = vm;
  return mm;
}

void BardMM_release( BardMM* mm )
{
  // Call clean_up() on all objects requiring it.
  //BardMM_clean_all( mm );

  BardMM_release_all( &mm->objects );
  //BardMM_release_all( &mm->objects_requiring_cleanup );
}

/*
int BardMM_should_collect_garbage( BardMM* mm )
{
  return (mm->gc_requested || mm->bytes_allocated_since_gc >= BARD_MM_BYTES_BEFORE_GC);
}

void BardMM_collect_garbage( BardMM* mm )
{
  mm->gc_requested = 0;
  mm->bytes_allocated_since_gc = 0;

  BardMM_trace_references( mm );

  BardMM_release_unused_objects( mm );
  BardMM_clean_unused_objects( mm );
}


void BardMM_trace_references( BardMM* mm )
{
  // Trace singletons
  {
    BardType** cur = ((BardType**) mm->vm->types.data) - 1;
    int count = mm->vm->types.count + 1;
    while (--count)
    {
      BardType* type = *(++cur);
      if (type)
      {
        BardObject* obj = type->singleton;
        if (obj && obj->size >= 0) BardMM_trace( obj );
      }
    }
  }

  // Trace literal strings
  {
    BardObject** cur = (BardObject**) (mm->vm->literal_String_values.data) - 1;
    int count = mm->vm->literal_String_values.count + 1;
    while (--count)
    {
      BardObject* obj = *(++cur);
      if (obj->size >= 0) BardMM_trace( obj );
    }
  }

  // Trace global refs
  //{
  //  BardGlobalRef* cur = global_refs;
  //  while (cur)
  //  {
  //    BardObject* obj = cur->object;
  //    if (obj->size >= 0) trace( obj );
  //    cur = cur->next;
  //  }
  //}

  // Trace string table
}

void BardMM_trace( struct BardObject* obj )
{
  // Requires: obj != null && obj->size >= 0
  // Note: obj->size is complemented when traced.
  obj->size = ~obj->size;

  //BardTraceFn trace_fn = obj->type->trace_fn;
  //if (trace_fn)
  //{
  //  trace_fn( obj );
  //}
}


void BardMM_clean_all( BardMM* mm )
{
  //BardObject* cur = mm->objects_requiring_cleanup;
  //mm->objects_requiring_cleanup = NULL;

  //while (cur)
  //{
  //  BardObject* next = (BardObject*) cur->next_allocation;
  //  cur->clean_up();
  //  cur->next_allocation = objects;
  //  objects = cur;
  //  cur = next;
  //}
}
*/

void BardMM_release_all( BardObject** list )
{
  BardObject* cur = *list;
  *list = NULL;

  while (cur)
  {
//printf("Freeing %s\n",cur->type->name);
    BardObject* next = (BardObject*) cur->next_allocation;
    BardAllocator_free( cur, cur->size );
    cur = next;
  }
}

/*
void BardMM_clean_unused_objects( BardMM* mm )
{
  BardObject* cur = mm->objects_requiring_cleanup;

  // Note: both objects and objects_requiring_cleanup can change 
  // outside this method if a clean_up() call produces new objects.
  mm->objects_requiring_cleanup = NULL;

  while (cur)
  {
    BardObject* next = cur->next_allocation;
    if (cur->size < 0)
    {
      // Was traced through so we'll keep it.
      cur->size = ~cur->size;
      cur->next_allocation = mm->objects_requiring_cleanup;
      mm->objects_requiring_cleanup = cur;
    }
    else
    {
      //cur->clean_up();  // Could create new objects
      cur->next_allocation = mm->objects;
      mm->objects = cur;
    }
    cur = next;
  }
}

void BardMM_release_unused_objects( BardMM* mm )
{
  BardObject* cur = mm->objects;
  mm->objects = NULL;
  BardObject* survivors = NULL;

  while (cur)
  {
    BardObject* next = (BardObject*) cur->next_allocation;
    if (cur->size < 0)
    {
      cur->size = ~cur->size;
      cur->next_allocation = survivors;
      survivors = cur;
    }
    else
    {
//printf( "Freeing %llx\n", (long long int) cur );
//printf( "Freeing type %s of size %d\n", cur->type->name, cur->size );
      BardAllocator_free( cur, cur->size );
    }
    cur = next;
  }

  mm->objects = survivors;
}
*/

BardObject* BardMM_create_object( BardMM* mm, BardType* type, int size )
{
  if (size == -1) size = type->object_size;
  mm->bytes_allocated_since_gc += size;

  BardObject* obj = (BardObject*) BardAllocator_allocate( size );
  obj->type = type;
  obj->size = size;

  /*
  if (type->flags & BARD_TYPE_REQUIRES_CLEANUP)
  {
    obj->next_allocation = mm->objects_requiring_cleanup;
    mm->objects_requiring_cleanup = obj;
  }
  else
  */
  {
    obj->next_allocation = mm->objects;
    mm->objects = obj;
  }
  return obj;
}

/*
//=============================================================================
//  Bard
//=============================================================================
int bard_initialized = 0;

static void Bard_reset()
{
  // atexit() callback registered by Bard_init
  if ( !bard_initialized ) return;

  while (bard_vm_list)
  {
    BardVM* next_vm = bard_vm_list->next_vm;

    if (bard_vm_list->dynamically_allocated) BardVM_free( bard_vm_list );
    else BardVM_release( bard_vm_list );

    bard_vm_list = next_vm;
  }

  BardInt32List_release( &bard_loader_id_table );
  BardInt32List_release( &bard_loader_String_table );
  BardReal64List_release( &bard_loader_Real64_table );
  BardReal32List_release( &bard_loader_Real32_table );
  BardInt64List_release(  &bard_loader_Int64_table );
  //BardInt32List_release( &bard_loader_op_name_ids );
  //BardInt32List_release( &bard_loader_op_mappings );

  bard_initialized = 0;

  BardAllocator_reset();
}

void Bard_init()
{
  // Call before doing anything else with Bard.  Defined in bard_mm.c.
  if (bard_initialized) return;
  bard_initialized = 1;

  BardAllocator_initialize();
  atexit( Bard_reset );
}


*/
