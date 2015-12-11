#ifndef BARD_VM_H
#define BARD_VM_H

#include <setjmp.h>

#include "Bard.h"

typedef union BardStackValue
{
  BardObject*   object;
  BardReal      real;
  BardInt64     integer64;
  BardInteger   integer;
} BardStackValue;

typedef struct BardStackFrame
{
  BardMethod*     m;
  BardInteger*    ip;
  BardStackValue* fp;
} BardStackFrame;

typedef struct BardNativeMethodInfo
{
  struct BardNativeMethodInfo* next_method_info;
  char*  type_context_name;
  char*  signature;
  BardNativeMethod function_pointer;

} BardNativeMethodInfo;

void BardNativeMethodInfo_free( BardNativeMethodInfo* info );

struct BardReader;

//=============================================================================
//  BardVM
//=============================================================================
typedef struct BardVM
{
  BardMM           mm;

  char**           identifiers;
  int              identifier_count;

  BardType**       types;
  int              type_count;

  BardMethod**     methods;
  int              method_count;

  BardNativeMethodInfo* native_methods;

  jmp_buf*         current_error_handler;
  int              error_code;
  char*            error_message;

  BardType*        type_Real;
  BardType*        type_Integer;
  BardType*        type_Character;
  BardType*        type_Byte;
  BardType*        type_Logical;

  BardType*        type_Object;
  BardType*        type_String;
  BardType*        type_StringBuilder;

  BardType*        type_LogicalList;
  BardType*        type_ByteList;
  BardType*        type_CharacterList;
  BardType*        type_IntegerList;
  BardType*        type_RealList;
  BardType*        type_ObjectList;
  BardType*        type_StringList;

  BardType*        type_Array_of_Logical;
  BardType*        type_Array_of_Byte;
  BardType*        type_Array_of_Character;
  BardType*        type_Array_of_Integer;
  BardType*        type_Array_of_Real;
  BardType*        type_Array_of_Object;

  BardType*        type_Random;

  BardType*        main_class;

  BardInteger*     code;
  int              code_size;
  BardObject**     literal_strings;
  int              literal_string_count;
  BardReal*        literal_reals;
  int              literal_real_count;
  BardObject**     singletons;

  BardStackValue*  sp;
  BardStackFrame*  current_frame;

  BardStackValue   stack[BARD_STACK_SIZE];
  BardStackFrame   call_stack[BARD_STACK_SIZE];

  char**           filenames;
  int              filename_count;

  unsigned char*   line_info;
  int              line_info_count;

  struct BardEventQueue*  event_queue;
  BardType*               type_EventManager;
  BardMethod*             m_key_buffer;
  BardMethod*             m_value_buffer;
  BardMethod*             m_queue_argument_count;
  BardMethod*             m_queue_string_argument;
  BardMethod*             m_queue_real_argument;
  BardMethod*             m_queue_integer_argument;
  BardMethod*             m_dispatch;

  BardType*               type_TaskManager;
  BardMethod*             m_update_task_manager;

  int BardList_data_offset;
  int BardList_count_offset;

  int BardString_characters_offset;
  int BardString_hash_code_offset;

  /*
  BardObject* objects;
  BardObject* objects_requiring_cleanup;
  //struct BardGlobalRef* global_refs;

  int gc_requested;
  int bytes_allocated_since_gc;
  */
} BardVM;

BardVM*   BardVM_create();  // calls init() automatically
void      BardVM_free( BardVM* vm );

BardVM*   BardVM_init( BardVM* vm );
BardVM*   BardVM_release( BardVM* vm );

BardVM*   BardVM_register_native_method( BardVM* vm, const char* type_context_name, const char* signature, BardNativeMethod fn_ptr );

int       BardVM_load_bc_file( BardVM* vm, const char* filename );
int       BardVM_load_bc_data( BardVM* vm, const char* filename, char* data, int count, int free_data_on_close );

void      BardVM_throw_error( BardVM* vm, int code, const char* message );

void      BardVM_set_sizes_and_offsets( BardVM* vm );
void      BardVM_organize( BardVM* vm );
void      BardVM_create_singletons( BardVM* vm );

BardType* BardVM_find_type( BardVM* vm, const char* type_name );
BardType* BardVM_must_find_type( BardVM* vm, const char* type_name );

int         BardVM_invoke_by_method_signature( BardVM* vm, BardType* type, const char* method_signature );
int         BardVM_invoke( BardVM* vm, BardType* type, BardMethod* m );

int         BardVM_read_line_info_integer( BardVM* vm, int* index );
BardMethod* BardVM_find_method_at_ip( BardVM* vm, int ip );
int         BardVM_find_line_info_at_ip( BardVM* vm, BardMethod* m, int ip, char** filename_pointer, int* line_number_pointer );

void        BardVM_create_main_object( BardVM* vm );

int         BardVM_update( BardVM* vm );
void        BardVM_dequeue_event_string( BardVM* vm, BardObject* string_builder );

BardObject* BardVM_collect_references( BardVM* vm );
/*
void        BardVM_call_clean_up( BardObject* list );
void        BardVM_free_object_list( BardObject* cur );
BardObject* BardVM_create_object( BardType* type, int size );
void        BardVM_collect_garbage();

*/


#endif

