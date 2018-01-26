//=============================================================================
//  BossList.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

struct List : Object
{
  // PROPERTIES
  Integer count;
  Array*  array;

  // METHODS
  List* add_byte( Byte value );
  List* add_character( Character value );
  List* add_float( Float value );
  List* add_integer( Integer value );
  List* add_logical( Logical value );
  List* add_long( Long value );
  List* add_object( const char* value );
  List* add_object( Object* value );
  List* add_real( Real value );
  List* clear() { count = 0; return this; }
  List* ensure_capacity( Integer required_capacity );
  List* reserve( Integer additional_capacity );
};

}; // namespace Boss
