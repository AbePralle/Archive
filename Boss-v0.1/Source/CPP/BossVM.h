//=============================================================================
//  BossVM.h
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#pragma once

#include "Boss.h"

namespace Boss
{

struct VM
{
  // PROPERTIES
  Allocator   allocator;

  Value       stack[ BOSS_STACK_SIZE ];
  Value*      sp;
  Value*      sp_limit;

  Cmd*        token;  // filename/position for stuff created internally

  StringBuilder error_message;
  Integer       error_line;
  String*       error_filepath;
  JumpBuffer*   jump_buffer;

  Type*     type_Real;
  Type*     type_Float;
  Type*     type_Long;
  Type*     type_Integer;
  Type*     type_Character;
  Type*     type_Byte;
  Type*     type_Logical;
  Type*     type_Object;
  Type*     type_String;
  Type*     type_ObjectArray;
  Type*     type_ObjectList;
  Type*     type_Table;
  Type*     type_TableEntry;

  Logical       have_new_system_objects;
  SystemObject* system_objects;
  Object*       objects;

  // Program Info
  Table*       routines;
  Table*       strings;
  SystemList*  routine_list;
  SystemTable* routines_by_name;

  Method*      main_routine;

  // METHODS
  VM();
  ~VM();

  void      collect_garbage();
  String*   consolidate( const char* st );
  String*   consolidate( Character* characters, Integer count );
  String*   consolidate( String* st ) { return consolidate(st->characters,st->count); }
  String*   consolidate( StringBuilder& buffer ) { return consolidate(buffer.characters,buffer.count); }
  Method*   create_routine( Cmd* t, String* name );

  Logical   execute( const char* utf8=0 );
  Method*   get_routine( Cmd* t, const char* name );

  VM&       push_real( Real value );
  VM&       push_float( Float value );
  VM&       push_long( Long value );
  VM&       push_integer( Integer value );
  VM&       push_character( Character value );
  VM&       push_byte( Byte value );
  VM&       push_logical( Logical value );

  Real      pop_real();
  Float     pop_float();
  Long      pop_long();
  Integer   pop_integer();
  Character pop_character();
  Byte      pop_byte();
  Logical   pop_logical();

  Array*    create_array( Type* array_type, Integer count );

  List*     create_list( Type* list_type, Integer initial_capacity=10 );
  List*     create_object_list( Integer initial_capacity=10 ) { return create_list(type_ObjectList,initial_capacity); }

  String* create_string( const char* utf8_data, Integer utf8_count=-1 );
  String* create_string( Character* data, Integer count );
  String* create_string( Integer character_count );
  Table*  create_table( Integer bin_count=32 );

  Logical load( const char* filepath );
  void    log_error();
};

}; // namespace Boss
