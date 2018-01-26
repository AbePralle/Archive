//=============================================================================
//  BossStringBuilder.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

struct StringBuilder
{
  VM*        vm;
  Character* characters;
  Integer    capacity;
  Integer    count;
  Character  internal_buffer[1024];

  StringBuilder( Integer initial_capacity=1024 );
  ~StringBuilder();


  StringBuilder& log();

  StringBuilder& print( Character value );
  StringBuilder& print( Character* values, Integer len );
  StringBuilder& print( Integer value );
  StringBuilder& print( Long value );
  StringBuilder& print( Object* value );
  StringBuilder& print( Real value );
  StringBuilder& print( const char value );
  StringBuilder& print( const char* value );
  StringBuilder& print( StringBuilder& other ) { return print(other.characters,other.count); }

  StringBuilder& println();
  StringBuilder& println( Character value );
  StringBuilder& println( Character* values, Integer len );
  StringBuilder& println( Integer value );
  StringBuilder& println( Long value );
  StringBuilder& println( Object* value );
  StringBuilder& println( Real value );
  StringBuilder& println( const char value );
  StringBuilder& println( const char* value );
  StringBuilder& println( StringBuilder& other ) { return println(other.characters,other.count); }

  StringBuilder& reserve( Integer additional );
};

}; // namespace Boss
