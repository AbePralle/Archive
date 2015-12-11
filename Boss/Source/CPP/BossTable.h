//=============================================================================
//  BossTable.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

//-----------------------------------------------------------------------------
//  Table
//-----------------------------------------------------------------------------
struct TableReader;

struct TableEntry : Object
{
  Object*     key;
  Object*     value;
  Integer     hash_code;
  TableEntry* next_entry;
};

struct Table : Object
{
  Array*  bins;
  Integer bin_count;
  Integer bin_mask;
  Integer count;

  Table*  init( Integer initial_bin_count );

  Table*  clear();

  TableEntry* find( const char* key );
  TableEntry* find( Character* key, Integer count );
  TableEntry* find( Object* key );

  Object* get( const char* key );
  Object* get( Character* key, Integer count );
  Object* get( Object* key );

  TableReader reader();

  Object* remove( const char* key );
  Object* remove( Object* key );

  Table& set( const char* key, Object* value );
  Table& set( Object* key, Object* value );
};

struct TableReader
{
  Table*      table;
  TableEntry* cur_entry;
  Integer     index;
  Integer     bin_index;

  TableReader( Table* table );
  void        advance();
  Logical     has_another() { return !!cur_entry; }
  TableEntry* read();

};

}; // namespace Boss
