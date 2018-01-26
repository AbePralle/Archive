//=============================================================================
//  BossSystemObject.cpp
//
//  2015.08.29 by Abe Pralle
//=============================================================================
#include "Boss.h"

namespace Boss
{

void* SystemObject::operator new( size_t size, VM* vm )
{
  SystemObject* obj = (SystemObject*) vm->allocator.allocate( (int) size );
  obj->vm = vm;
  obj->size = (int) size;
  obj->reference_count = 0;
  obj->next_allocation = vm->system_objects;
  vm->system_objects = obj;
  vm->have_new_system_objects = true;
  return obj;
}

void  SystemObject::operator delete( void* obj )
{
  ((SystemObject*)obj)->vm->allocator.free( obj, ((SystemObject*)obj)->size );
}

}; // namespace Boss
