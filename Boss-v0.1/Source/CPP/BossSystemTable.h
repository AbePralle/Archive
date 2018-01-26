//=============================================================================
//  BossSystemTable.h
//
//  2010.08.27 by Abe Pralle
//=============================================================================
#pragma once
#include "Boss.h"

namespace Boss
{

//-----------------------------------------------------------------------------
//  SystemTable
//-----------------------------------------------------------------------------
struct SystemTableReader;

struct SystemTableEntry
{
  String*           key;
  void*             value;
  SystemTableEntry* next_entry;

  SystemTableEntry( String* key, void* value, SystemTableEntry* next_entry=0 );
};

struct SystemTable
{
  VM*                vm;
  SystemTableEntry** bins;
  Integer            bin_count;
  Integer            bin_mask;
  Integer            count;

  SystemTable( VM* vm, Integer initial_bin_count=16 ) { init(vm,initial_bin_count); }
  ~SystemTable();

  SystemTable* init( VM* vm, Integer initial_bin_count=16 );

  SystemTable*  clear();

  SystemTableEntry* find( const char* key );
  SystemTableEntry* find( Character* key, Integer count );
  SystemTableEntry* find( Character* key, Integer count, Integer hash_code );
  SystemTableEntry* find( String* key );

  void* get( const char* key );
  void* get( Character* key, Integer count );
  void* get( Character* key, Integer count, Integer hash_code );
  void* get( String* key );

  SystemTableReader reader();

  void* remove( const char* key );
  void* remove( String* key );
  void* remove_another();

  SystemTable& set( const char* key, void* value );
  SystemTable& set( String* key, void* value );
};

struct SystemTableReader
{
  SystemTable*      table;
  SystemTableEntry* cur_entry;
  Integer           index;
  Integer           bin_index;

  SystemTableReader( SystemTable* table );
  void              advance();
  Logical           has_another() { return !!cur_entry; }
  SystemTableEntry* read();
};

}; // namespace Boss
