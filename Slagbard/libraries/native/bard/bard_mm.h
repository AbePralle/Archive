#ifndef BARD_MM_H
#define BARD_MM_H
//=============================================================================
//  bard_mm.h
//
//  Bard Memory Manager
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
//    2010.12.24 / Abe Pralle - v3.2 revamp
//    2008.12.17 / Abe Pralle - Created
//=============================================================================

#ifndef BARDMM_SMALL_OBJECT_PAGE_SIZE
#  define BARDMM_PERMALLOC_PAGE_SIZE (512*1024)
#endif

#define BARDMM_NUM_GENERATIONS 4
#define BARDMM_NUM_ALLOC_POOLS 4

struct BardGlobalRef
{
  BardObject* object;

  BardGlobalRef() : object(NULL) { }
  BardGlobalRef( BardObject* object );
  ~BardGlobalRef();

  void operator=( BardObject* obj );

  BardObject* operator*() { return object; }
  BardObject* operator->() { return object; }
  operator BardObject*() { return object; }
  operator BardArray*() { return (BardArray*) object; }
  operator BardString*() { return (BardString*) object; }
};

struct BardObjectManager
{
  int count;
  int limit;
  int gen;
  BardObject*       objects;
  BardObjectManager* next_gen;

  BardObjectManager() : count(0), limit(1000), objects(NULL), next_gen(NULL) { }

  virtual ~BardObjectManager()
  {
    clean_up();
  }

  virtual void clean_up();

  void add( BardObject* object );
  void check_gc();

  virtual void delete_object( BardObject* obj );
  virtual void collect();

  void zero_reference_counts();
  void delete_unreferenced_objects();
};

struct BardObjectManagerWithCleanup : BardObjectManager
{
  void collect();
};

struct BardSmallObjectManager : BardObjectManager
{
  int bin_index;

  void clean_up()
  {
    objects = NULL;
    count = 0;
  }

  void delete_object( BardObject* obj );
};

struct BardMM
{
  BardObjectManager generations[BARDMM_NUM_GENERATIONS];
  BardSmallObjectManager generations32[BARDMM_NUM_GENERATIONS];
  BardSmallObjectManager generations64[BARDMM_NUM_GENERATIONS];
  BardSmallObjectManager generations96[BARDMM_NUM_GENERATIONS];
  BardSmallObjectManager generations128[BARDMM_NUM_GENERATIONS];
  BardObjectManagerWithCleanup objects_requiring_cleanup;
  BardObject* objects_pending_cleanup;

  AllocList<char*> permalloc_pages;
  char* cur_permalloc_page;
  int   available_permalloc_size;

  BardObject* allocation_pools[BARDMM_NUM_ALLOC_POOLS];

  ArrayList<BardGlobalRef*> global_refs;
  ArrayList<BardWeakRef*>   weak_refs;
  BardObject*               local_refs[256];
  BardObject**              local_ref_limit;  // hard-coded to 256 in init()
  BardObject**              local_ref_ptr;

  int stack_retain_count;

  bool initialized;
  bool force_gc;

  BardMM() { initialized=false; init(); }
  ~BardMM();

  void init();
  void shut_down();

  void check_gc();
  void gc();
  void retain_stack();
  void release_stack();
  void zero_weak_refs_to_unreferenced_objects();
  void zero_all_reference_counts();
  void trace_accessible_objects();
  static void trace( BardObject* object );

  BardObject* create_object( BardTypeInfo* type, int byte_size );
  BardArray*  create_array( BardTypeInfo* type, int count );

  BardObject* permalloc( int bytes );
};

extern BardMM mm;
extern BardGlobalRef bard_main_object;

//=============================================================================
//  BardLocalRef
//=============================================================================
struct BardLocalRef
{
  BardObject** stack_pos;

  BardLocalRef( BardObject* object )
  {
    *(stack_pos = --mm.local_ref_ptr) = object;
#if defined(BARD_VM)
    if (stack_pos < mm.local_refs) bard_throw_fatal_error("Local ref stack limit exceeded.");
#endif
  }

  BardLocalRef( const BardLocalRef& other )
  {
    *(stack_pos = --mm.local_ref_ptr) = *(other.stack_pos);
#if defined(BARD_VM)
    if (stack_pos < mm.local_refs) bard_throw_fatal_error("Local ref stack limit exceeded.");
#endif
  }

  ~BardLocalRef()
  {
    ++mm.local_ref_ptr;
  }

  void operator=( BardObject* object )
  {
    *stack_pos = object;
  }

  BardObject* operator*() { return *stack_pos; }
  BardObject* operator->() { return *stack_pos; }

  operator BardObject*() { return (BardObject*) *stack_pos; }
  operator BardArray*() { return (BardArray*) *stack_pos; }
  operator BardString*() { return (BardString*) *stack_pos; }
};


#endif

