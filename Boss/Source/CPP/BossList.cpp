//=============================================================================
//  BossList.cpp
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"

namespace Boss
{

// Note: while it seems like the following could be simplified to [count++] instead
// of [count];++count;, for some reason [count++] will actually increment the count BEFORE
// reserve() is called.
List* List::add_byte( Byte value ) { reserve(1)->array->bytes[count] = value; ++count; return this; }
List* List::add_character( Character value ) { reserve(1)->array->characters[count] = value; ++count; return this; }
List* List::add_float( Float value ) { reserve(1)->array->floats[count] = value; ++count; return this; }
List* List::add_integer( Integer value ) { reserve(1)->array->integers[count] = value; ++count; return this; }
List* List::add_logical( Logical value ) { reserve(1)->array->logicals[count] = value; ++count; return this; }
List* List::add_long( Long value ) { reserve(1)->array->longs[count] = value; ++count; return this; }
List* List::add_object( const char* value ) { return add_object( type->vm->create_string(value) ); }
List* List::add_object( Object* value ) { reserve(1)->array->objects[count] = value; ++count; return this; }
List* List::add_real( Real value ) { reserve(1)->array->reals[count] = value; ++count; return this; }

List* List::ensure_capacity( Integer required_capacity )
{
  return reserve( required_capacity - array->count );
}

List* List::reserve( Integer additional_capacity )
{
  Integer required_capacity = count + additional_capacity;
  if (array->count >= required_capacity)
  {
    return this;
  }

  Integer double_capacity = array->count * 2;
  if (double_capacity > required_capacity) required_capacity = double_capacity;

  Array* new_array = type->vm->create_array( array->type, required_capacity );
  memcpy( new_array->bytes, array->bytes, count*array->type->element_size() );
  array = new_array;

  return this;
}

}; // namespace Boss
