//=============================================================================
//  BossSystemTable.cpp
//
//  2010.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"
using namespace Boss;

//-----------------------------------------------------------------------------
//  SystemTableEntry
//-----------------------------------------------------------------------------
SystemTableEntry::SystemTableEntry( String* key, void* value, SystemTableEntry* next_entry )
    : key(key), value(value), next_entry(next_entry)
{
}

//-----------------------------------------------------------------------------
//  SystemTable
//-----------------------------------------------------------------------------
SystemTable::~SystemTable()
{
  clear();
  delete bins;
}

SystemTable* SystemTable::init( VM* vm, Integer initial_bin_count )
{
  this->vm = vm;

  // Ensure power-of-2 bin count
  bin_count = 1;
  while (bin_count < initial_bin_count) bin_count <<= 1;

  bin_mask = bin_count - 1;
  bins = new SystemTableEntry*[ bin_count ];
  for (int i=bin_count; --i>=0; )
  {
    bins[i] = 0;
  }

  return this;
}

SystemTable* SystemTable::clear()
{
  while (remove_another()) {}
  return this;
}

SystemTableEntry* SystemTable::find( const char* key )
{
  Integer hash_code = calculate_hash_code( key );

  SystemTableEntry* cur = bins[ hash_code & bin_mask ];
  while (cur)
  {
    if (cur->key->hash_code == hash_code && cur->key->equals(key))
    {
      return cur;
    }
    cur = cur->next_entry;
  }

  return 0;
}

SystemTableEntry* SystemTable::find( Character* key, Integer count )
{
  return find( key, count, calculate_hash_code(key, count) );
}

SystemTableEntry* SystemTable::find( Character* key, Integer count, Integer hash_code )
{
  SystemTableEntry* cur  = bins[ hash_code & bin_mask ];
  while (cur)
  {
    if (cur->key->hash_code == hash_code && cur->key->equals(key,count))
    {
      return cur;
    }
    cur = cur->next_entry;
  }

  return 0;
}

SystemTableEntry* SystemTable::find( String* key )
{
  return find( key->characters, key->count, key->hash_code );
}


void* SystemTable::get( const char* key )
{
  Integer hash_code = calculate_hash_code( key );

  SystemTableEntry* cur = bins[ hash_code & bin_mask ];
  while (cur)
  {
    if (cur->key->hash_code == hash_code && cur->key->equals(key))
    {
      return cur->value;
    }
    cur = cur->next_entry;
  }

  return 0;
}

void* SystemTable::get( Character* key, Integer count )
{
  return get( key, count, calculate_hash_code(key, count) );
}


void* SystemTable::get( Character* key, Integer count, Integer hash_code )
{
  SystemTableEntry* cur  = bins[ hash_code & bin_mask ];
  while (cur)
  {
    if (cur->key->hash_code == hash_code && cur->key->equals(key,count))
    {
      return cur->value;
    }
    cur = cur->next_entry;
  }

  return 0;
}

void* SystemTable::get( String* key )
{
  return get( key->characters, key->count, key->hash_code );
}

void* SystemTable::remove( const char* key )
{
  Integer hash_code = calculate_hash_code( key );

  SystemTableEntry* prev = 0;
  SystemTableEntry* cur  = bins[ hash_code & bin_mask ];
  while (cur)
  {
    if (cur->key->hash_code == hash_code && cur->key->equals(key))
    {
      if (prev)
      {
        // Remove in tail
        prev->next_entry = cur->next_entry;
      }
      else
      {
        // Remove head
        bins[ hash_code & bin_mask ] = cur->next_entry;
      }
      --count;
      void* result = cur->value;
      delete cur;
      return result;
    }
    prev = cur;
    cur = cur->next_entry;
  }

  return 0;
}

void* SystemTable::remove( String* key )
{
  Integer hash_code = key->hash_code;

  SystemTableEntry* prev = 0;
  SystemTableEntry* cur  = bins[ hash_code & bin_mask ];
  while (cur)
  {
    if (cur->key->hash_code == hash_code && cur->key->equals(key))
    {
      if (prev)
      {
        // Remove in tail
        prev->next_entry = cur->next_entry;
      }
      else
      {
        // Remove head
        bins[ hash_code & bin_mask ] = cur->next_entry;
      }
      --count;
      void* result = cur->value;
      delete cur;
      return result;
    }
    prev = cur;
    cur = cur->next_entry;
  }

  return 0;
}

void* SystemTable::remove_another()
{
  if (count == 0) return 0;

  for (int i=bin_count; --i>=0; )
  {
    SystemTableEntry* cur;
    if ((cur=bins[i]))
    {
      bins[i] = cur->next_entry;
      void* result = cur->value;
      delete cur;
      --count;
      return result;
    }
  }

  return 0;
}

SystemTableReader SystemTable::reader()
{
  return SystemTableReader( this );
}

SystemTable& SystemTable::set( const char* key, void* value )
{
  Integer hash_code = calculate_hash_code( key );

  SystemTableEntry* cur  = bins[ hash_code & bin_mask ];
  SystemTableEntry* prev = 0;
  while (cur)
  {
    if (cur->key->hash_code == hash_code && cur->key->equals(key))
    {
      // Replace existing entry
      cur->value = value;
      return *this;
    }
    prev = cur;
    cur = cur->next_entry;
  }

  // New entry
  cur = new SystemTableEntry( vm->consolidate(key), value );

  if (prev)
  {
    // End of bin list
    prev->next_entry = cur;
  }
  else
  {
    // First item in list
    bins[ hash_code & bin_mask ] = cur;
  }

  ++count;

  return *this;
}


SystemTable& SystemTable::set( String* key, void* value )
{
  Integer hash_code = key->hash_code;

  SystemTableEntry* cur  = bins[ hash_code & bin_mask ];
  SystemTableEntry* prev = 0;
  while (cur)
  {
    if (cur->key->hash_code == hash_code && cur->key->equals(key))
    {
      // Replace existing entry
      cur->value = value;
      return *this;
    }
    prev = cur;
    cur = cur->next_entry;
  }

  // New entry
  cur = new SystemTableEntry( vm->consolidate(key), value );

  if (prev)
  {
    // End of bin list
    prev->next_entry = cur;
  }
  else
  {
    // First item in list
    bins[ hash_code & bin_mask ] = cur;
  }

  ++count;

  return *this;
}

SystemTableReader::SystemTableReader( SystemTable* table )
  : table(table), cur_entry(0), index(-1), bin_index(-1)
{
  advance();
}

void SystemTableReader::advance()
{
  if (++index >= table->count)
  {
    cur_entry = 0;
    return;
  }

  if (cur_entry)
  {
    cur_entry = cur_entry->next_entry;
    if (cur_entry) return;
  }

  while ( !cur_entry )
  {
    cur_entry = table->bins[++bin_index];
  }
}

SystemTableEntry* SystemTableReader::read()
{
  SystemTableEntry* result = cur_entry;
  advance();
  return result;
}
