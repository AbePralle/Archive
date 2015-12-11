#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "Bard.h"

BardMethod* BardMethod_create( int global_index, BardType* type_context, char* name, int parameter_count, BardType* return_type, int local_count )
{
  BardMethod* m = (BardMethod*) BARD_ALLOCATE( sizeof(BardMethod) );
  bzero( m, sizeof(BardMethod) );

  m->global_index = global_index;
  m->type_context = type_context;
  m->name = name;
  m->parameter_count = parameter_count;
  m->return_type = return_type;
  m->local_count = local_count;

  if (local_count)
  {
    m->local_types = (BardType**) BARD_ALLOCATE( sizeof(BardType*) * local_count );
  }

  return m;
}

void BardMethod_free( BardMethod* m )
{
  if (m->exception_handlers)
  {
    int i;
    for (i=0; i<m->exception_handler_count; ++i)
    {
      BardExceptionHandlerInfo* handler = m->exception_handlers + i;
      if (handler->catches) BARD_FREE( handler->catches );
    }
    BARD_FREE( m->exception_handlers );
    m->exception_handlers = NULL;
    m->exception_handler_count = 0;
  }

  if (m->tags)
  {
    int count = m->tag_count;
    int i;
    for (i=0; i<count; ++i)
    {
      BARD_FREE( m->tags[i] );
    }
    BARD_FREE( m->tags );
    m->tags = NULL;
  }

  if (m->local_types)
  {
    BARD_FREE( m->local_types );
    m->local_types = NULL;
  }

  //if (m->signature)
  //{
  //  BARD_FREE( m->signature );
  //  m->signature = NULL;
  //}

  if (m->call_signature)
  {
    BARD_FREE( m->call_signature );
    m->call_signature = NULL;
  }

  BARD_FREE( m );
}

//char* BardMethod_signature( BardMethod* m )
//{
//  BardMethod_create_signature_if_necessary( m );
//  return m->signature;
//}

char* BardMethod_call_signature( BardMethod* m )
{
  BardMethod_create_call_signature_if_necessary( m );
  return m->call_signature;
}

//void BardMethod_create_signature_if_necessary( BardMethod* m )
//{
//  int sig_len;
//  if (m->signature) return;
//
//  sig_len = BardMethod_call_signature_count( m );
//  if (m->return_type) sig_len += strlen( m->return_type->name ) + 2;
//
//  m->signature = BARD_ALLOCATE( sig_len+1 );
//  BardMethod_compose_call_signature( m, m->signature );
//  strcat( m->signature, "->" );
//  strcat( m->signature, m->return_type->name );
//
//  m->signature_hash_code = BardCString_hash_code( m->signature );
//}

void BardMethod_create_call_signature_if_necessary( BardMethod* m )
{
  int sig_len;
  if (m->call_signature) return;

  sig_len = BardMethod_call_signature_count( m );

  m->call_signature = BARD_ALLOCATE( sig_len+1 );
  BardMethod_compose_call_signature( m, m->call_signature );

  m->call_signature_hash_code = BardCString_hash_code( m->call_signature );
}

int BardMethod_call_signature_count( BardMethod* m )
{
  int sig_len = strlen(m->name) + 2;  // name + parens + null terminator
  if (m->parameter_count)
  {
    int i;
    sig_len += m->parameter_count - 1;  // a char for each comma 
    for (i=0; i<m->parameter_count; ++i)
    {
      sig_len += strlen(m->local_types[i]->name);
    }
  }
  return sig_len;
}

void BardMethod_compose_call_signature( BardMethod* m, char* buffer )
{
  strcpy( buffer, m->name );
  strcat( buffer, "(" );
  if (m->parameter_count)
  {
    int i;
    for (i=0; i<m->parameter_count; ++i)
    {
      if (i > 0) strcat( buffer, "," );
      strcat( buffer, m->local_types[i]->name );
    }
  }
  strcat( buffer, ")" );
}

void BardMethod_organize( BardMethod* m )
{
  int i;

  m->parameter_slot_count = 0;
  m->local_slot_count = 0;

  for (i=0; i<m->parameter_count; ++i)
  {
    m->parameter_slot_count += m->local_types[i]->stack_slots;
  }

  for (i=m->parameter_count; i<m->local_count; ++i)
  {
    m->local_slot_count += m->local_types[i]->stack_slots;
  }

  m->standard_return_slot_delta = (m->return_type) ? 0 : m->type_context->stack_slots;
//printf( "%s::%s has %d local slots\n", m->type_context->name, BardMethod_call_signature(m), m->local_slot_count );
}

