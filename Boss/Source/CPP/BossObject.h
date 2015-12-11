//=============================================================================
//  BossObject.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

struct Object : Allocation
{
  Type*   type;

  Logical equals( Character* other, Integer count ) { return type->equals(this,other,count); }
  Logical equals( const char* other ) { return type->equals(this,other); }
  Logical equals( Object* other ) { return type->equals(this,other); }
  Integer get_hash_code() { return type->get_hash_code(this); }
  void    print();
  void    println();
  void    trace() { type->trace(this); }
};

}; // namespace Boss
