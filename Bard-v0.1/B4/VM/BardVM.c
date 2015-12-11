#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "Bard.h"
#include "BardOpcodes.h"

//=============================================================================
//  GLOBALS
//=============================================================================
BardVM* Bard_active_vm = NULL;


//=============================================================================
//  BardNativeMethodInfo
//=============================================================================
void BardNativeMethodInfo_free( BardNativeMethodInfo* info )
{
  while (info)
  {
    BardNativeMethodInfo* next = info->next_method_info;

    BARD_FREE( info->type_context_name );
    BARD_FREE( info->signature );
    BARD_FREE( info );

    info = next;
  }
}

//=============================================================================
//  BardVM
//=============================================================================
BardVM* BardVM_create()
{
  BardVM* vm = BARD_ALLOCATE( sizeof(BardVM) );
  return BardVM_init( vm );
}

void BardVM_free( BardVM* vm )
{
  BARD_FREE( BardVM_release(vm) );
}

BardVM* BardVM_init( BardVM* vm )
{
  bzero( vm, sizeof(BardVM) );

  BardMM_init( &vm->mm, vm );

  vm->sp = vm->stack + BARD_STACK_SIZE;

  // Set up "current frame" to valid if meaningless values.
  vm->current_frame = (vm->call_stack + BARD_STACK_SIZE) - 1;
  vm->current_frame->fp = vm->sp;
  vm->current_frame->ip = vm->code - 1;

  vm->event_queue = BardEventQueue_create();

  return vm;
}

BardVM* BardVM_release( BardVM* vm )
{
  int i;
  BardMM_release( &vm->mm );

  if (vm->line_info)
  {
    BARD_FREE( vm->line_info );
    vm->line_info = NULL;
    vm->line_info_count = 0;
  }

  BardNativeMethodInfo_free( vm->native_methods );

  if (vm->error_message)
  {
    BARD_FREE( vm->error_message );
    vm->error_message = NULL;
  }

  if (vm->singletons)
  {
    BARD_FREE( vm->singletons );
    vm->singletons = NULL;
  }

  if (vm->literal_reals)
  {
    BARD_FREE( vm->literal_reals );
    vm->literal_reals = NULL;
  }

  if (vm->literal_strings)
  {
    BARD_FREE( vm->literal_strings );
    vm->literal_strings = NULL;
  }

  if (vm->code)
  {
    BARD_FREE( vm->code );
    vm->code = NULL;
    vm->code_size = 0;
  }

  for (i=0; i<vm->method_count; ++i)
  {
    BardMethod_free( (BardMethod*) vm->methods[i] );
  }
  BARD_FREE( vm->methods );
  vm->methods = NULL;

  for (i=0; i<vm->type_count; ++i)
  {
    BardType_free( (BardType*) vm->types[i] );
  }
  BARD_FREE( vm->types );
  vm->types = NULL;

  for (i=0; i<vm->identifier_count; ++i)
  {
    BARD_FREE( vm->identifiers[i] );
  }
  BARD_FREE( vm->identifiers );
  vm->identifiers = NULL;

  if (vm->event_queue)
  {
    BardEventQueue_free( vm->event_queue );
    vm->event_queue = NULL;
  }

  return vm;
}

BardVM* BardVM_register_native_method( BardVM* vm, const char* type_context_name, const char* signature, BardNativeMethod fn_ptr )
{
  BardNativeMethodInfo* info = (BardNativeMethodInfo*) BARD_ALLOCATE( sizeof(BardNativeMethodInfo) );

  bzero( info, sizeof(BardNativeMethodInfo) );

  info->type_context_name = BardCString_duplicate( type_context_name );
  info->signature   = BardCString_duplicate( signature );
  info->function_pointer = fn_ptr;

  info->next_method_info = vm->native_methods;
  vm->native_methods = info;

  return vm;
}

int BardVM_load_bc_file( BardVM* vm, const char* filename )
{
  BardReader* reader = BardReader_create_with_filename( filename );
  if ( !reader ) return 0;

  {
    int result = BardLoader_load_bc_from_reader( vm, reader );
    BardReader_free( reader );
    return result;
  }
}

int BardVM_load_bc_data( BardVM* vm, const char* filename, char* data, int count, int free_data_on_close )
{
  BardReader* reader = BardReader_create_with_data( filename, data, count, free_data_on_close );
  if ( !reader ) return 0;

  {
    int result = BardLoader_load_bc_from_reader( vm, reader );
    BardReader_free( reader );
    return result;
  }
}

void BardVM_throw_error( BardVM* vm, int code, const char* message )
{
  if (vm->error_message)
  {
    BARD_FREE( vm->error_message );
    vm->error_message = NULL;
  }

  vm->error_code = code;
  if (message)
  {
    vm->error_message = BARD_ALLOCATE( strlen(message)+1 );
    strcpy( vm->error_message, message );
  }

  if (vm->current_error_handler)
  {
    longjmp( *(vm->current_error_handler), code );
  }
  else
  {
    printf( "ERROR: No error handler set.\n" );
    printf( "ERROR: %s.\n", message );
  }
}

void BardVM_set_sizes_and_offsets( BardVM* vm )
{
  // Locate important types
  vm->type_Real      = BardVM_must_find_type( vm, "Real" );
  vm->type_Integer   = BardVM_must_find_type( vm, "Integer" );
  vm->type_Character = BardVM_must_find_type( vm, "Character" );
  vm->type_Byte      = BardVM_must_find_type( vm, "Byte" );
  vm->type_Logical   = BardVM_must_find_type( vm, "Logical" );

  vm->type_Object        = BardVM_must_find_type( vm, "Object" );
  vm->type_String        = BardVM_must_find_type( vm, "String" );
  vm->type_StringBuilder = BardVM_must_find_type( vm, "StringBuilder" );

  vm->type_Array_of_Logical   = BardVM_find_type( vm, "Array<<Logical>>" );
  vm->type_Array_of_Byte      = BardVM_find_type( vm, "Array<<Byte>>" );
  vm->type_Array_of_Character = BardVM_find_type( vm, "Array<<Character>>" );
  vm->type_Array_of_Integer   = BardVM_find_type( vm, "Array<<Integer>>" );
  vm->type_Array_of_Real      = BardVM_find_type( vm, "Array<<Real>>" );
  vm->type_Array_of_Object    = BardVM_find_type( vm, "Array<<Object>>" );

  vm->type_ByteList      = BardVM_find_type( vm, "Byte[]" );
  vm->type_LogicalList   = BardVM_find_type( vm, "Logical[]" );
  vm->type_CharacterList = BardVM_find_type( vm, "Character[]" );
  vm->type_IntegerList   = BardVM_find_type( vm, "Integer[]" );
  vm->type_RealList      = BardVM_find_type( vm, "Real[]" );
  vm->type_ObjectList    = BardVM_find_type( vm, "Object[]" );
  vm->type_StringList    = BardVM_find_type( vm, "String[]" );

  vm->type_Random        = BardVM_find_type( vm, "Random" );

  // Set each type's reference size
  {
    int i;
    for (i=0; i<vm->type_count; ++i)
    {
      BardType* type = vm->types[i];
      BardType_set_reference_sizes( type );
    }
  }

  // Set each type's property offsets
  {
    int i;
    for (i=0; i<vm->type_count; ++i)
    {
      BardType_set_up_settings( vm->types[i] );
      BardType_set_property_offsets( vm->types[i] );
    }
  }

  // Collect various methods and property offsets
  vm->BardList_data_offset = BardType_find_property( vm->type_ObjectList, "data" )->offset;
  vm->BardList_count_offset = BardType_find_property( vm->type_ObjectList, "count" )->offset;
}

void BardVM_organize( BardVM* vm )
{
  // Organize each type
  {
    int i;
    for (i=0; i<vm->type_count; ++i)
    {
      BardType* type = vm->types[i];
      BardType_organize( type );
    }
  }

  // Organize each method
  {
    int i;
    for (i=0; i<vm->method_count; ++i)
    {
      BardMethod* m = vm->methods[i];
      BardMethod_organize( m );
    }
  }

  // Assign native methods
  {
    {
      int i;
      for (i=0; i<vm->method_count; ++i)
      {
        BardMethod* m = vm->methods[i];
        if (BardMethod_is_native(m)) 
        {
          m->native_method = BardProcessor_on_unhandled_native_method;
          m->ip[1] = BARD_CREATE_OP_WITH_ARG_N( BARD_OP_NATIVE_CALL_INDEX_N, m->global_index );
          m->ip[2] = BARD_OP_NATIVE_RETURN;
        }
      }
    }

    BardNativeMethodInfo* cur = vm->native_methods;
    while (cur)
    {
      BardType* type = BardVM_find_type( vm, cur->type_context_name );
      if (type)
      {
        BardMethod* m = BardType_find_method( type, cur->signature );
        if (m)
        {
          m->local_slot_count = 0;
          m->native_method = cur->function_pointer;
          m->ip[1] = BARD_CREATE_OP_WITH_ARG_N( BARD_OP_NATIVE_CALL_INDEX_N, m->global_index );
          m->ip[2] = BARD_OP_NATIVE_RETURN;
        }
      }
      cur = cur->next_method_info;
    }
  }
}

void BardVM_create_singletons( BardVM* vm )
{
  // Create singletons
  vm->singletons = (BardObject**) BARD_ALLOCATE( sizeof(BardObject*) * vm->type_count );

  {
    // Create the actual objects
    int i;
    for (i=0; i<vm->type_count; ++i)
    {
      BardType* type = vm->types[i];
      BardObject* singleton = BardType_create_object( type );
      type->singleton = singleton;
      vm->singletons[i] = singleton;
    }

    // Call init_settings() on each singleton
    for (i=0; i<vm->type_count; ++i)
    {
      BardType* type = vm->types[i];
      BardMethod* m_init_settings = BardType_find_method( type, "init_settings()" );
      BardVM_push_object( vm, type->singleton );
      BardProcessor_invoke( vm, type, m_init_settings );
    }

    // Call init_defaults() on each singleton officially marked [singleton]
    for (i=0; i<vm->type_count; ++i)
    {
      BardType* type = vm->types[i];
      if ( BardType_is_singleton(type) )
      {
        BardVM_push_object( vm, type->singleton );
        BardProcessor_invoke( vm, type, type->m_init_defaults );
      }
    }

    // Call init() if it exists on each singleton officially marked [singleton]
    // that's not the main class.
    for (i=0; i<vm->type_count; ++i)
    {
      BardType* type = vm->types[i];
      if (BardType_is_singleton(type) && type != vm->main_class)
      {
        BardMethod* m_init = BardType_find_method( type, "init()" );
        if (m_init)
        {
          BardVM_push_object( vm, type->singleton );
          BardProcessor_invoke( vm, type, m_init );
          ++vm->sp;
        }
      }
    }
  }
}

BardType* BardVM_find_type( BardVM* vm, const char* type_name )
{
  int i = vm->type_count;
  while (--i >= 0)
  {
    BardType* cur_type = vm->types[i];
    if (0 == strcmp(cur_type->name,type_name)) return cur_type;
  }
  return NULL;
}

BardType* BardVM_must_find_type( BardVM* vm, const char* type_name )
{
  BardType* type;
  type = BardVM_find_type( vm, type_name );

  if ( !type )
  {
    char st[80];
    if (strlen(type_name) > 50) strcpy( st, "Missing required type." );
    else                        sprintf( st, "Missing required type %s.", type_name );
    BardVM_throw_error( vm, 1, st );
  }

  return type;
}

int BardVM_invoke_by_method_signature( BardVM* vm, BardType* type, const char* method_signature )
{
  BardMethod* m;

  if ( !type ) return 0;

  m = BardType_find_method( type, method_signature );
  if ( !m ) return 0;

  return BardVM_invoke( vm, type, m );
}

int BardVM_invoke( BardVM* vm, BardType* type, BardMethod* m )
{
  if ( !type || !m ) return 0;

  BardProcessor_invoke( vm, type, m );
  return (0 == vm->error_code);
}

int BardVM_read_line_info_integer( BardVM* vm, int* index )
{
  // 0: 0xxxxxxx - 0..127
  // 1: 1000xxxx - 1 byte follows (12 bits total, unsigned)
  // 2: 1001xxxx - 2 bytes follow (20 bits total, unsigned)
  // 3: 1010xxxx - 3 bytes follow (28 bits total, unsigned)
  // 4: 10110000 - 4 bytes follow (32 bits total, signed)
  // 5: 11xxxxxx - -64..-1
  int b1;
  
  b1 = vm->line_info[ (*index)++ ];

  if (b1 < 128)  return b1;          // encoding 0
  if (b1 >= 192) return (b1 - 256);  // encoding 5

  switch (b1 & 0x30)
  {
    case 0:  // encoding 1
      return ((b1 & 15) << 8) | vm->line_info[ (*index)++ ];

    case 16:  // encoding 2
      {
        int b2 = vm->line_info[ (*index)++ ];
        int b3 = vm->line_info[ (*index)++ ];
        return ((b1 & 15) << 16) | (b2 << 8) | b3;
      }

    case 32:  // encoding 3
      {
        int b2 = vm->line_info[ (*index)++ ];
        int b3 = vm->line_info[ (*index)++ ];
        int b4 = vm->line_info[ (*index)++ ];
        return ((b1 & 15) << 24) | (b2 << 16) | (b3 << 8) | b4;
      }

    case 48:  // encoding 4
      {
        int result = vm->line_info[ (*index)++ ];
        result = (result << 8) | vm->line_info[ (*index)++ ];
        result = (result << 8) | vm->line_info[ (*index)++ ];
        result = (result << 8) | vm->line_info[ (*index)++ ];
        return (BardInteger) result;
      }

    default:
      return -1;
  }
}

BardMethod* BardVM_find_method_at_ip( BardVM* vm, int ip )
{
  int i;
  for (i=0; i<vm->method_count; ++i)
  {
    BardMethod* m = vm->methods[i];
    if (ip >= m->ip_start && ip < m->ip_limit) return m;
  }
  return NULL;
}

int BardVM_find_line_info_at_ip( BardVM* vm, BardMethod* m, int ip, char** filename_pointer, int* line_number_pointer )
{
  if (m)
  {
    int line_info_index = m->line_info_index;

    int file_switch_count = BardVM_read_line_info_integer( vm, &line_info_index );
    int f;

    int file_index, cur_line;
    for (f=0; f<file_switch_count; ++f)
    {
      file_index = BardVM_read_line_info_integer( vm, &line_info_index );
      cur_line   = BardVM_read_line_info_integer( vm, &line_info_index );
      int cur_ip     = BardVM_read_line_info_integer( vm, &line_info_index );
      int delta_count = BardVM_read_line_info_integer( vm, &line_info_index );
      int d;
      for (d=0; d<delta_count; ++d)
      {
        int next_line = cur_line + BardVM_read_line_info_integer( vm, &line_info_index );
        int next_ip   = cur_ip   + BardVM_read_line_info_integer( vm, &line_info_index );
        if (ip < next_ip) break;

        cur_line = next_line;
        cur_ip = next_ip;
      }
      *filename_pointer = vm->filenames[ file_index ];
      *line_number_pointer = cur_line;
      return 1;
    }

    *filename_pointer = vm->filenames[ file_index ];
    *line_number_pointer = cur_line;

    return 1;
  }
  else
  {
    return 0;
  }
}

/*
void BardVM_shut_down( BardVM* vm )
{
  Bard_use_vm( vm );

  // Call clean_up once on every object requiring it.
  BardObject* list = vm->objects_requiring_cleanup;
  vm->objects_requiring_cleanup = NULL;

  BardVM_call_clean_up( list );

  // Free every object in the clean-up list (could have been
  // created from the initial clean-up call) and the regular list.
  BardVM_free_object_list( vm->objects_requiring_cleanup );
  BardVM_free_object_list( vm->objects );
  vm->objects_requiring_cleanup = NULL;
  vm->objects = NULL;

  BardOrderedTable_release( &vm->types );
  BardIDTable_release( &vm->id_table );

  Bard_use_vm( NULL );

  BARD_FREE( vm );
}

void BardVM_call_clean_up( BardObject* cur )
{
  while (cur)
  {
    BardObject* next = cur->next_allocation;
    cur->next_allocation = Bard_active_vm->objects;
    Bard_active_vm->objects = cur;
    cur = next;
  }
}

void BardVM_free_object_list( BardObject* cur )
{
  while (cur)
  {
//printf( "End: Freeing size %d\n", cur->size );
    BardObject* next = cur->next_allocation;
    BardAllocator_free( cur, cur->size );
    cur = next;
  }
}

BardObject* BardVM_create_object( BardType* type, int size )
{
  if (size == -1) size = type->object_size;

  BardObject* object = BardAllocator_allocate( size );
  object->type = type;
  object->size = size;

  if (type->qualifiers & BARD_TYPE_REQUIRES_CLEANUP)
  {
    object->next_allocation = Bard_active_vm->objects_requiring_cleanup;
    Bard_active_vm->objects_requiring_cleanup = object;
  }
  else
  {
    object->next_allocation = Bard_active_vm->objects;
    Bard_active_vm->objects = object;
  }

  Bard_active_vm->bytes_allocated_since_gc += size;

  return object;
}

void BardVM_collect_garbage()
{
  if (Bard_active_vm->gc_requested)
  {
    Bard_active_vm->gc_requested = 0;
  }
  else
  {
    if (Bard_active_vm->bytes_allocated_since_gc < BARD_BYTES_BEFORE_GC) return;
  }
  Bard_active_vm->bytes_allocated_since_gc = 0;

  // Trace through all references.

  // Free any unreferenced objects.
  {
    BardObject* remaining = NULL;
    BardObject* cur = Bard_active_vm->objects;
    while (cur)
    {
      BardObject* next = cur->next_allocation;
      if (cur->size < 0)
      {
        // Referenced; link into remaining list.
        cur->size = ~cur->size;
        cur->next_allocation = remaining;
        remaining = cur;
        cur = next;
      }
      else
      {
        // Unreferenced; free allocation.
        BardObject* next = cur->next_allocation;
        BardAllocator_free( cur, cur->size );
        cur = next;
      }
    }
    Bard_active_vm->objects = remaining;
  }


  // Call clean-up on any unreferenced objects requiring clean-up
  // and move those objects to the regular objects list.
}
*/

void BardVM_create_main_object( BardVM* vm )
{
  BardType*    main_class = vm->main_class;
  BardMethod*  m_init = BardType_find_method( main_class, "init()" );

  if (m_init)
  {
    BardVM_push_object( vm, main_class->singleton );
    BardVM_invoke( vm, main_class, m_init );
    BardVM_pop_discard( vm );
  }
}

int BardVM_update( BardVM* vm )
{
#ifdef NOCOMPILE
  if ( !vm->type_EventManager )
  {
    vm->type_EventManager = BardVM_find_type( vm, "EventManager" );

    vm->m_key_buffer = BardType_find_method( vm->type_EventManager, "key_buffer()" );
    vm->m_value_buffer = BardType_find_method( vm->type_EventManager, "value_buffer()" );
    vm->m_queue_argument_count = BardType_find_method( vm->type_EventManager, "queue_argument_count(Integer)" );
    vm->m_queue_string_argument = BardType_find_method( vm->type_EventManager, "queue_string_argument()" );
    vm->m_queue_real_argument = BardType_find_method( vm->type_EventManager, "queue_real_argument(Real)" );
    vm->m_queue_integer_argument = BardType_find_method( vm->type_EventManager, "queue_integer_argument(Integer)" );
    vm->m_dispatch = BardType_find_method( vm->type_EventManager, "dispatch()" );

    vm->type_TaskManager = BardVM_find_type( vm, "TaskManager" );
    vm->m_update_task_manager = BardType_find_method( vm->type_TaskManager, "update()" );
  }

  // TEST
  //BardEventQueue_begin_event( vm->event_queue, "update", 0 );
  //BardEventQueue_begin_event( vm->event_queue, "display_change", 3 );
  //BardEventQueue_add_integer( vm->event_queue, "width",  320 );
  //BardEventQueue_add_integer( vm->event_queue, "height", 480 );
  //BardEventQueue_add_real( vm->event_queue, "joy", 3.2 );

  // Transfer each event to Bard
  {
    BardEventQueue* q = vm->event_queue;
    int event_count = q->count;
    int event_index;
    BardObject* event_manager = vm->type_EventManager->singleton;
    BardObject* key_buffer;
    BardObject* value_buffer;

    BardVM_push_object( vm, event_manager );
    BardProcessor_invoke( vm, vm->type_EventManager, vm->m_key_buffer );
    key_buffer = BardVM_pop_object( vm );

    BardVM_push_object( vm, event_manager );
    BardProcessor_invoke( vm, vm->type_EventManager, vm->m_value_buffer );
    value_buffer = BardVM_pop_object( vm );

    for (event_index=0; event_index < event_count; ++event_index)
    {
      int arg_count;
      int arg_index;

      BardVM_dequeue_event_string( vm, key_buffer );  // event type
      arg_count = q->integers[ q->integer_position++ ];  // arg count

      // EventManager.queue_argument_count( n )  # implicit key_buffer as type
      BardVM_push_object( vm, event_manager );
      BardVM_push_integer( vm, arg_count );
      BardProcessor_invoke( vm, vm->type_EventManager, vm->m_queue_argument_count );

      for (arg_index=0; arg_index<arg_count; ++arg_index)
      {
        BardVM_dequeue_event_string( vm, key_buffer );

        BardVM_push_object( vm, event_manager );

        switch (q->integers[ q->integer_position++ ])
        {
          case BARD_EVENT_DATA_STRING:
            BardVM_dequeue_event_string( vm, value_buffer );
            BardProcessor_invoke( vm, vm->type_EventManager, vm->m_queue_string_argument );
            break;

          case BARD_EVENT_DATA_REAL:
            BardVM_push_real( vm, q->reals[ q->real_position++ ] );
            BardProcessor_invoke( vm, vm->type_EventManager, vm->m_queue_real_argument );
            break;

          case BARD_EVENT_DATA_INTEGER:
            BardVM_push_integer( vm, q->integers[ q->integer_position++ ] );
            BardProcessor_invoke( vm, vm->type_EventManager, vm->m_queue_integer_argument );
            break;

          default:
            printf( "[Internal] Arg type error in BardVM_update()\n" );
        }
      }
    }

    // Clear the queue before dispatching the events since there's a good chance that
    // the event handlers will cause more events to be queued.
    BardEventQueue_clear( vm->event_queue );

    BardVM_push_object( vm, event_manager );
    BardProcessor_invoke( vm, vm->type_EventManager, vm->m_dispatch );
  }

  // Call update() on TaskManager
  BardVM_push_object( vm, vm->type_TaskManager->singleton );
  BardProcessor_invoke( vm, vm->type_TaskManager, vm->m_update_task_manager );

  return BardVM_pop_logical(vm);  // TaskManager.update() returns false when all tasks are finished.
#endif
  return 0;
}

/*
void BardVM_dequeue_event_string( BardVM* vm, BardObject* string_builder )
{
  BardEventQueue* q = vm->event_queue;
  BardCharacter* src = (q->characters + q->integers[q->integer_position++]) - 1;
  int count = q->integers[ q->integer_position++ ];
  BardCharacter* dest;

  string_builder->count = 0;
  BardString_ensure_capacity( string_builder, count );
  string_builder->count = count;

  dest = BardString_character_array(vm,string_builder)->character_data - 1;
  while (--count >= 0)
  {
    *(++dest) = *(++src);
  }

  BardString_set_hash_code( string_builder );
}

BardObject* BardVM_collect_references( BardVM* vm )
{
  BardObject* references = NULL;

  // Collect singletons
  {
    int singleton_count = vm->type_count;
    BardObject** cur_singleton = vm->singletons + singleton_count;
    while (--singleton_count >= 0)
    {
      (*(--cur_singleton))->next_reference = references;
      references = *cur_singleton;
    }
  }

  return references;
}
*/
