#ifndef BARD_EVENT_QUEUE_H
#define BARD_EVENT_QUEUE_H

#define BARD_EVENT_DATA_STRING  1
#define BARD_EVENT_DATA_REAL    2
#define BARD_EVENT_DATA_INTEGER 3

typedef struct BardEventQueue
{
  BardInteger*   integers;
  BardCharacter* characters;
  BardReal*      reals;

  int          count;

  int          integer_count;
  int          character_count;
  int          real_count;

  int          integer_capacity;
  int          character_capacity;
  int          real_capacity;

  int          integer_position;
  int          character_position;
  int          real_position;
} BardEventQueue;

// PUBLIC
BardEventQueue* BardEventQueue_create();
void            BardEventQueue_free( BardEventQueue* queue );

void            BardEventQueue_clear( BardEventQueue* queue );

void            BardEventQueue_begin_event( BardEventQueue* queue, const char* event_type, int arg_count );
void            BardEventQueue_add_c_string( BardEventQueue* queue, const char* key, const char* value );
void            BardEventQueue_add_real( BardEventQueue* queue, const char* key, BardReal value );
void            BardEventQueue_add_integer( BardEventQueue* queue, const char* key, BardInteger value );

// PRIVATE
void            BardEventQueue_write_c_string( BardEventQueue* queue, const char* value );
void            BardEventQueue_write_real( BardEventQueue* queue, BardReal value );
void            BardEventQueue_write_integer( BardEventQueue* queue, BardInteger value );

#endif // BARD_EVENT_QUEUE_H
