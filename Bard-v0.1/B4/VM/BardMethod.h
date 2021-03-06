#ifndef BARD_METHOD_H
#define BARD_METHOD_H

#include "Bard.h"

typedef struct BardExceptionHandlerCatchInfo
{
  BardType*    catch_type;
  BardInteger* handler_ip;
  int local_slot_index;
} BardExceptionHandlerCatchInfo;

typedef struct BardExceptionHandlerInfo
{
  BardInteger* ip_start;
  BardInteger* ip_limit;
  int                            catch_count;
  BardExceptionHandlerCatchInfo* catches;
} BardExceptionHandlerInfo;

typedef struct BardMethod
{
  BardType*  type_context;
  char*      name;           // Does not need to be freed - comes from VM's session id_list.
  //char*      signature; // May be null
  //int        signature_hash_code;
  char*      call_signature; // May be null
  int        call_signature_hash_code;
  BardType** local_types;
  BardType*  return_type;

  char**     tags;
  int        tag_count;

  int        attributes;
  int        global_index;

  int        parameter_count;
  int        local_count;       // # of locals vars

  int        parameter_slot_count;
  int        local_slot_count;   // total # of local stack slots used for local vars
  int        standard_return_slot_delta;  // 0 for methods with return values, # of context slots to pop for nil methods

  int*       ip;  // as actual pointer
  int        ip_start;  // zero-based
  int        ip_limit;  // zero-based
  int        line_info_index;

  BardNativeMethod native_method;

  int                       exception_handler_count;
  BardExceptionHandlerInfo* exception_handlers;

} BardMethod;

BardMethod* BardMethod_create( int global_index, BardType* type_context, char* name, int parameter_count, BardType* return_type, int local_count );
void        BardMethod_free( BardMethod* m );

//char*       BardMethod_signature( BardMethod* m );
char*       BardMethod_call_signature( BardMethod* m );

//void        BardMethod_create_signature_if_necessary( BardMethod* m );
void        BardMethod_create_call_signature_if_necessary( BardMethod* m );

int         BardMethod_call_signature_count( BardMethod* m );
void        BardMethod_compose_call_signature( BardMethod* m, char* buffer );

void        BardMethod_organize( BardMethod* m );

#endif // BARD_METHOD_H
