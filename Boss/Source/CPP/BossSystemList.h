//=============================================================================
//  BossSystemList.h
//
//  2010.08.27 by Abe Pralle
//=============================================================================
#pragma once
#include "Boss.h"

namespace Boss
{

struct SystemList
{
  void**  data;
  Integer count;
  Integer capacity;

  SystemList( Integer capacity=10 );
  ~SystemList();

  SystemList* add( void* value );
  SystemList* clear();
  SystemList* discard( Integer i1, Integer i2 );
  SystemList* discard_from( Integer i1 );

  inline void*& at( Integer index ) { return data[index]; }
  inline void*& operator[]( Integer index ) { return data[index]; }

  void  remove( void* value );
  void* remove_at( Integer index );
  void* remove_last();

  SystemList* reserve( Integer additional_count );
  SystemList* ensure_capacity( Integer c );
};

}; // namespace Boss
