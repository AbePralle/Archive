//=============================================================================
//  BossTable.cpp
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"

namespace Boss
{

//-----------------------------------------------------------------------------
//  Table
//-----------------------------------------------------------------------------
Table* Table::init( Integer initial_bin_count )
{
  // Ensure power-of-2 bin count
  bin_count = 1;
  while (bin_count < initial_bin_count) bin_count <<= 1;

  bin_mask = bin_count - 1;

  VM* vm = type->vm;
  bins = vm->create_array( vm->type_ObjectArray, bin_count );

  return this;
}

Table* Table::clear()
{
  count = 0;
  for (Integer i=0; i<bin_count; ++i)
  {
    bins->objects[i] = 0;
  }
  return this;
}

TableEntry* Table::find( const char* key )
{
  Integer hash_code = calculate_hash_code( key );

  TableEntry* cur = (TableEntry*) bins->objects[ hash_code & bin_mask ];
  while (cur)
  {
    if (cur->hash_code == hash_code && cur->key && cur->key->equals(key))
    {
      return cur;
    }
    cur = cur->next_entry;
  }

  return 0;
}

TableEntry* Table::find( Character* key, Integer count )
{
  Integer hash_code = calculate_hash_code( key, count );

  TableEntry* cur  = (TableEntry*) bins->objects[ hash_code & bin_mask ];
  while (cur)
  {
    if (cur->hash_code == hash_code && cur->key && cur->key->equals(key,count))
    {
      return cur;
    }
    cur = cur->next_entry;
  }

  return 0;
}

TableEntry* Table::find( Object* key )
{
  Integer hash_code = 0;
  if (key) hash_code = key->get_hash_code();

  TableEntry* cur  = (TableEntry*)(bins->objects[ hash_code & bin_mask ]);
  while (cur)
  {
    if (cur->hash_code == hash_code && cur->key && cur->key->equals(key))
    {
      return cur;
    }
    cur = cur->next_entry;
  }

  return 0;
}


Object* Table::get( const char* key )
{
  Integer hash_code = calculate_hash_code( key );

  TableEntry* cur = (TableEntry*) bins->objects[ hash_code & bin_mask ];
  while (cur)
  {
    if (cur->hash_code == hash_code && cur->key && cur->key->equals(key))
    {
      return cur->value;
    }
    cur = cur->next_entry;
  }

  return 0;
}

Object* Table::get( Character* key, Integer count )
{
  Integer hash_code = calculate_hash_code( key, count );

  TableEntry* cur  = (TableEntry*) bins->objects[ hash_code & bin_mask ];
  while (cur)
  {
    if (cur->hash_code == hash_code && cur->key && cur->key->equals(key,count))
    {
      return cur->value;
    }
    cur = cur->next_entry;
  }

  return 0;
}


Object* Table::get( Object* key )
{
  Integer hash_code = 0;
  if (key) hash_code = key->get_hash_code();

  TableEntry* cur  = (TableEntry*)(bins->objects[ hash_code & bin_mask ]);
  while (cur)
  {
    if (cur->hash_code == hash_code && cur->key && cur->key->equals(key))
    {
      return cur->value;
    }
    cur = cur->next_entry;
  }

  return 0;
}

Object* Table::remove( const char* key )
{
  Integer hash_code = calculate_hash_code( key );

  TableEntry* prev = 0;
  TableEntry* cur  = (TableEntry*) bins->objects[ hash_code & bin_mask ];
  while (cur)
  {
    if (cur->hash_code == hash_code && cur->key->equals(key))
    {
      if (prev)
      {
        // Remove in tail
        prev->next_entry = cur->next_entry;
      }
      else
      {
        // Remove head
        bins->objects[ hash_code & bin_mask ] = cur->next_entry;
      }
      --count;
      return cur->value;
    }
    prev = cur;
    cur = cur->next_entry;
  }

  return 0;
}

Object* Table::remove( Object* key )
{
  Integer hash_code = 0;
  if (key) hash_code = key->get_hash_code();

  TableEntry* prev = 0;
  TableEntry* cur  = (TableEntry*)(bins->objects[ hash_code & bin_mask ]);
  while (cur)
  {
    if (cur->hash_code == hash_code && cur->key && cur->key->equals(key))
    {
      if (prev)
      {
        // Remove in tail
        prev->next_entry = cur->next_entry;
      }
      else
      {
        // Remove head
        bins->objects[ hash_code & bin_mask ] = cur->next_entry;
      }
      --count;
      return cur->value;
    }
    prev = cur;
    cur = cur->next_entry;
  }

  return 0;
}

TableReader Table::reader()
{
  return TableReader( this );
}

Table& Table::set( const char* key, Object* value )
{
  Integer hash_code = calculate_hash_code( key );

  TableEntry* cur  = (TableEntry*) bins->objects[ hash_code & bin_mask ];
  TableEntry* prev = 0;
  while (cur)
  {
    if (cur->hash_code == hash_code && cur->key && cur->key->equals(key))
    {
      // Replace existing entry
      cur->value = value;
      return *this;
    }
    prev = cur;
    cur = cur->next_entry;
  }

  // New entry
  VM* vm = type->vm;
  cur = (TableEntry*) vm->type_TableEntry->create_object();
  cur->hash_code = hash_code;
  cur->key = vm->create_string( key );
  cur->value = value;

  if (prev)
  {
    // End of bin list
    prev->next_entry = cur;
  }
  else
  {
    // First item in list
    bins->objects[ hash_code & bin_mask ] = cur;
  }

  ++count;

  return *this;
}


Table& Table::set( Object* key, Object* value )
{
  Integer hash_code = 0;
  if (key) hash_code = key->get_hash_code();

  TableEntry* cur  = (TableEntry*) bins->objects[ hash_code & bin_mask ];
  TableEntry* prev = 0;
  while (cur)
  {
    if (cur->hash_code == hash_code && cur->key && cur->key->equals(key))
    {
      // Replace existing entry
      cur->value = value;
      return *this;
    }
    prev = cur;
    cur = cur->next_entry;
  }

  // New entry
  VM* vm = type->vm;
  cur = (TableEntry*) vm->type_TableEntry->create_object();
  cur->hash_code = hash_code;
  cur->key = key;
  cur->value = value;

  if (prev)
  {
    // End of bin list
    prev->next_entry = cur;
  }
  else
  {
    // First item in list
    bins->objects[ hash_code & bin_mask ] = cur;
  }

  ++count;

  return *this;
}

//-----------------------------------------------------------------------------
//  TableReader
//-----------------------------------------------------------------------------
TableReader::TableReader( Table* table )
  : table(table), cur_entry(0), index(-1), bin_index(-1)
{
  advance();
}

void TableReader::advance()
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
    cur_entry = (TableEntry*) table->bins->objects[++bin_index];
  }
}

TableEntry* TableReader::read()
{
  TableEntry* result = cur_entry;
  advance();
  return result;
}

}; // namespace Boss
