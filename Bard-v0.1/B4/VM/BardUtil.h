#ifndef BARD_UTIL_H
#define BARD_UTIL_H

#include <setjmp.h>

//=============================================================================
//  Macros
//=============================================================================
#define BARD_TRY(vm) { jmp_buf local_jump_buf; vm->current_error_handler = &local_jump_buf; if ( !setjmp(local_jump_buf) ) {
#define BARD_CATCH_ERROR } else {
#define BARD_END_TRY(vm) } vm->current_error_handler = NULL; }

//=============================================================================
//  BardCString
//=============================================================================
int   BardCString_hash_code( const char* st );
int   BardCString_find( const char* st, char ch );
char* BardCString_duplicate( const char* st );
 
//=============================================================================
//  BardPointerList
//=============================================================================
typedef struct BardPointerList
{
  void** data;
  int    capacity;
  int    count;
} BardPointerList;

BardPointerList* BardPointerList_create();
void             BardPointerList_free( BardPointerList* list );

BardPointerList* BardPointerList_init( BardPointerList* list );
void             BardPointerList_release( BardPointerList* list );

BardPointerList* BardPointerList_clear( BardPointerList* list );
BardPointerList* BardPointerList_free_elements( BardPointerList* list );

void  BardPointerList_ensure_capacity( BardPointerList* list, int min_capacity );
void  BardPointerList_add( BardPointerList* list, void* value );

//=============================================================================
//  BardUtil
//=============================================================================
BardReal BardUtil_time();

#endif // BARD_UTIL_H

