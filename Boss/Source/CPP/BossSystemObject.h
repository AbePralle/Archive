//=============================================================================
//  BossSystemObject.h
//
//  2015.08.29 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

struct SystemObject : Allocation
{
  VM* vm;

  virtual ~SystemObject() {}

  static void* operator new( size_t size, VM* vm );
  static void  operator delete( void *obj );
};

}; // namespace Boss
