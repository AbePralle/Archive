#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bard.h"

BardEventQueue* BardEventQueue_create()
{
  BardEventQueue* queue = BARD_ALLOCATE( sizeof(BardEventQueue) );
  bzero( queue, sizeof(BardEventQueue) );

  queue->integer_capacity = 256;
  queue->character_capacity = 1024;
  queue->real_capacity = 128;

  queue->integers = (BardInteger*)    BARD_ALLOCATE( queue->integer_capacity * sizeof(BardInteger) );
  queue->characters = (BardCharacter*) BARD_ALLOCATE( queue->integer_capacity * sizeof(BardCharacter) );
  queue->reals = (BardReal*) BARD_ALLOCATE( queue->integer_capacity * sizeof(BardReal) );

  return queue;
}

void BardEventQueue_free( BardEventQueue* queue )
{
  BARD_FREE( queue->integers );
  BARD_FREE( queue->characters );
  BARD_FREE( queue->reals );
  BARD_FREE( queue );
}

void BardEventQueue_clear( BardEventQueue* queue )
{
  queue->count = 0;

  queue->integer_count = 0;
  queue->character_count = 0;
  queue->real_count = 0;

  queue->integer_position = 0;
  queue->character_position = 0;
  queue->real_position = 0;
}

void BardEventQueue_begin_event( BardEventQueue* queue, const char* event_type, int arg_count )
{
  ++queue->count;
  BardEventQueue_write_c_string( queue, event_type );
  BardEventQueue_write_integer( queue, arg_count );
}

void BardEventQueue_add_c_string( BardEventQueue* queue, const char* key, const char* value )
{
  BardEventQueue_write_c_string( queue, key );
  BardEventQueue_write_integer( queue, BARD_EVENT_DATA_STRING );
  BardEventQueue_write_c_string( queue, value );
}

void BardEventQueue_add_real( BardEventQueue* queue, const char* key, BardReal value )
{
  BardEventQueue_write_c_string( queue, key );
  BardEventQueue_write_integer( queue, BARD_EVENT_DATA_REAL );
  BardEventQueue_write_real( queue, value );
}

void BardEventQueue_add_integer( BardEventQueue* queue, const char* key, BardInteger value )
{
  BardEventQueue_write_c_string( queue, key );
  BardEventQueue_write_integer( queue, BARD_EVENT_DATA_INTEGER );
  BardEventQueue_write_integer( queue, value );
}

void BardEventQueue_write_c_string( BardEventQueue* queue, const char* value )
{
  int len = strlen( value );
  BardEventQueue_write_integer( queue, queue->character_count );  // starting index
  BardEventQueue_write_integer( queue, len );

  int required_capacity = queue->character_count + len;
  if (required_capacity > queue->character_capacity)
  {
    queue->character_capacity *= 2;
    if (queue->character_capacity < required_capacity) queue->character_capacity = required_capacity;
    BardCharacter* new_data = (BardCharacter*) BARD_ALLOCATE( queue->character_capacity * sizeof(BardCharacter) );
    memcpy( new_data, queue->characters, queue->character_count * sizeof(BardCharacter) );
    BARD_FREE( queue->characters );
    queue->characters = new_data;
  }

  {
    BardCharacter* dest = (queue->characters + queue->character_count) - 1;
    int i;
    for (i=0; i<len; ++i) *(++dest) = value[i];
    queue->character_count += len;
  }
}

void BardEventQueue_write_real( BardEventQueue* queue, BardReal value )
{
  if (queue->real_count == queue->real_capacity)
  {
    queue->real_capacity *= 2;
    BardReal* new_data = (BardReal*) BARD_ALLOCATE( queue->real_capacity * sizeof(BardReal) );
    memcpy( new_data, queue->reals, queue->real_count * sizeof(BardReal) );
    BARD_FREE( queue->reals );
    queue->reals = new_data;
  }

  queue->reals[ queue->real_count++ ] = value;
}

void BardEventQueue_write_integer( BardEventQueue* queue, BardInteger value )
{
  if (queue->integer_count == queue->integer_capacity)
  {
    queue->integer_capacity *= 2;
    BardInteger* new_data = (BardInteger*) BARD_ALLOCATE( queue->integer_capacity * sizeof(BardInteger) );
    memcpy( new_data, queue->integers, queue->integer_count * sizeof(BardInteger) );
    BARD_FREE( queue->integers );
    queue->integers = new_data;
  }

  queue->integers[ queue->integer_count++ ] = value;
}

