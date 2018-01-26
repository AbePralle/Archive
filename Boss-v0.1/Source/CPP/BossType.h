//=============================================================================
//  BossType.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

struct Type
{
  VM*        vm;
  char*      name;
  Integer    object_size;
  Attributes attributes;

  Type( VM* vm, const char* name, Integer object_size, Attributes attributes=Attributes() );
  virtual ~Type();

  virtual Type*   array_type() { return 0; }
          Object* create_object( Integer object_size=-1 );
  virtual Integer element_size() { return 0; }
  virtual Logical equals( Object* a, Character* b, Integer count ) { return 0; }
  virtual Logical equals( Object* a, const char* b ) { return 0; }
  virtual Logical equals( Object* a, Object* b ) { return (a == b); }
  virtual Integer get_hash_code( Object* object ) { return 0; }
  virtual void    print( Object* object, StringBuilder& buffer );
  virtual void    trace( Object* object );
};

struct TypeReal : Type
{
  TypeReal( VM* vm );
};

struct TypeFloat : Type
{
  TypeFloat( VM* vm );
};

struct TypeLong : Type
{
  TypeLong( VM* vm );
};

struct TypeInteger : Type
{
  TypeInteger( VM* vm );
};

struct TypeCharacter : Type
{
  TypeCharacter( VM* vm );
};

struct TypeByte : Type
{
  TypeByte( VM* vm );
};

struct TypeLogical : Type
{
  TypeLogical( VM* vm );
};

struct TypeObject : Type
{
  TypeObject( VM* vm );
};

struct TypeString : Type
{
  TypeString( VM* vm );

  Logical equals( Object* a, Character* b, Integer count );
  Logical equals( Object* a, const char* b );
  Logical equals( Object* a, Object* b );
  Integer get_hash_code( Object* object );
  void    print( Object* object, StringBuilder& buffer );
};

struct TypeTable : Type
{
  TypeTable( VM* vm );

  void print( Object* object, StringBuilder& buffer );
  void trace( Object* object );
};

struct TypeTableEntry : Type
{
  TypeTableEntry( VM* vm );
};

// Arrays
struct TypeObjectArray : Type
{
  TypeObjectArray( VM* vm );

  Integer element_size() { return sizeof(Object*); }
  void    print( Object* object, StringBuilder& buffer );
  void    trace( Object* object );
};

// Lists
struct TypeObjectList : Type
{
  TypeObjectList( VM* vm );

  Type*   array_type();
  void    print( Object* object, StringBuilder& buffer );
  void    trace( Object* object );
};

}; // namespace Boss
