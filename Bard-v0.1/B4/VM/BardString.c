#include <stdio.h>
#include <string.h>

#include "Bard.h"

//=============================================================================
//  BardString
//=============================================================================
BardString* BardString_create_with_size( BardVM* vm, int size )
{
  int total_size = sizeof(BardString) + sizeof(BardCharacter) * size;
  BardString* obj = (BardString*) BardMM_create_object( &vm->mm, vm->type_String, total_size );
  obj->count = size;
  return obj;
}

BardString* BardString_create_with_c_string( BardVM* vm, const char* st )
{
  return BardString_create_with_c_string_and_length( vm, st, -1 );
}

BardString* BardString_create_with_c_string_and_length( BardVM* vm, const char* st, int size )
{
  if (size == -1) size = strlen(st);
  BardString* obj = BardString_create_with_size( vm, size );

  {
    const char*    src = (st - 1);
    BardCharacter* dest = obj->characters - 1;
    int count = size;
    while (--count >= 0)
    {
      *(++dest) = *(++src);
    }
  }

  return BardString_set_hash_code( obj );
}

BardString* BardString_set_hash_code( BardString* obj )
{
  int hash_code = 0;
  int count = obj->count;
  BardCharacter* cur = obj->characters - 1;

  while (--count >= 0)
  {
    // hash_code = hash_code * 7 + ch
    hash_code = ((hash_code << 3) - hash_code) + *(++cur);
  }

  obj->hash_code = hash_code;

  return obj;
}

char* BardString_to_c_string( BardString* st, char* destination_buffer, int destination_buffer_capacity )
{
  BardCharacter* src = st->characters;

  int count = st->count;
  if (count >= destination_buffer_capacity) count = destination_buffer_capacity - 1;

  destination_buffer[count] = 0;
  while (--count >= 0)
  {
    destination_buffer[count] = (char) src[count];
  }
  
  return destination_buffer;
}


void BardString_print( BardString* st )
{
  BardCharacter* src = st->characters - 1;
  int count = st->count;
  while (--count >= 0)
  {
    putc( (char) *(++src), stdout );
  }
}

/*
BardObject* BardString_write_string( BardObject* st, BardObject* other )
{
  BardVM* vm = st->type->vm;
  BardCharacter* src;
  BardCharacter* dest;
  int hash_code;
  int count, necessary_capacity;
  count = BardString_count(vm,other);
  if ( !count ) return st;

  hash_code = BardString_hash_code(vm,st);

  necessary_capacity = BardString_count(vm,st) + count;
  if (necessary_capacity > BardString_capacity(vm,st))
  {
    int desired_capacity = BardString_capacity(vm,st) << 1;
    if (necessary_capacity > desired_capacity) desired_capacity = necessary_capacity;
    BardString_ensure_capacity( st, desired_capacity );
  }

  src = BardString_character_array(vm,other)->character_data - 1;
  dest = (BardString_character_array(vm,st)->character_data + BardString_count(vm,st)) - 1;
  BardString_count(vm,st) += count;

  while (--count >= 0)
  {
    int ch = *(++src);
    *(++dest) = ch;
    hash_code = BARD_STRING_HASH_ADD( hash_code, ch );
  }

  BardString_hash_code(vm,st) = hash_code;

  return st;
}
*/

