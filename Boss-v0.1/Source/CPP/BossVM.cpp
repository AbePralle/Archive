//=============================================================================
//  BossVM.cpp
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"

namespace Boss
{

VM::VM() : jump_buffer(0), have_new_system_objects(0), system_objects(0), objects(0)
{
  sp = sp_limit = (stack + BOSS_STACK_SIZE);

  type_Real = new TypeReal(this);
  type_Float = new TypeFloat(this);
  type_Long = new TypeLong(this);
  type_Integer = new TypeInteger(this);
  type_Character = new TypeCharacter(this);
  type_Byte = new TypeByte(this);
  type_Logical = new TypeLogical(this);
  type_Object = new TypeObject(this);
  type_String = new TypeString(this);
  type_ObjectArray  = new TypeObjectArray(this);
  type_ObjectList   = new TypeObjectList(this);
  type_Table        = new TypeTable(this);
  type_TableEntry   = new TypeTableEntry(this);

  strings = create_table( 128 );
  strings->retain();

  routines = create_table( 64 );
  routines->retain();

  routine_list = new SystemList();
  routines_by_name = new SystemTable(this);

  token = new(this,consolidate("[INTERNAL]"),0) CmdSymbol(KEYWORD_METHOD, "method");
  main_routine = create_routine( token, consolidate("main") );
}

VM::~VM()
{
  for (int i=routine_list->count; --i>=0; )
  {
    delete (Method*) routine_list->at(i);
  }
  delete routine_list;
  delete routines_by_name;

  delete type_Real;
  delete type_Float;
  delete type_Long;
  delete type_Integer;
  delete type_Character;
  delete type_Byte;
  delete type_Logical;
  delete type_Object;
  delete type_String;
  delete type_ObjectArray;
  delete type_ObjectList;
  delete type_Table;
  delete type_TableEntry;
}

VM& VM::push_real( Real value )
{
  if (--sp >= stack) sp->as_real = value;
  else               ++sp;
  return *this;
}

VM& VM::push_float( Float value )
{
  if (--sp >= stack) sp->as_float = value;
  else               ++sp;
  return *this;
}

VM& VM::push_long( Long value )
{
  if (--sp >= stack) sp->as_long = value;
  else               ++sp;
  return *this;
}

VM& VM::push_integer( Integer value )
{
  if (--sp >= stack) sp->as_integer = value;
  else               ++sp;
  return *this;
}

VM& VM::push_character( Character value )
{
  if (--sp >= stack) sp->as_character = value;
  else               ++sp;
  return *this;
}

VM& VM::push_byte( Byte value )
{
  if (--sp >= stack) sp->as_byte = value;
  else               ++sp;
  return *this;
}

VM& VM::push_logical( Logical value )
{
  if (--sp >= stack) sp->as_logical = value;
  else               ++sp;
  return *this;
}

Real VM::pop_real()
{
  if (sp < sp_limit) return (sp++)->as_real;
  else               return 0;
}

Float VM::pop_float()
{
  if (sp < sp_limit) return (sp++)->as_float;
  else               return 0;
}

Long VM::pop_long()
{
  if (sp < sp_limit) return (sp++)->as_long;
  else               return 0;
}

Integer VM::pop_integer()
{
  if (sp < sp_limit) return (sp++)->as_integer;
  else               return 0;
}

Character VM::pop_character()
{
  if (sp < sp_limit) return (sp++)->as_character;
  else               return 0;
}

Byte VM::pop_byte()
{
  if (sp < sp_limit) return (sp++)->as_byte;
  else               return 0;
}

Logical VM::pop_logical()
{
  if (sp < sp_limit) return (sp++)->as_logical;
  else               return 0;
}


void VM::collect_garbage()
{
  // Collect standard Object garbage
  Allocation* survivors = 0;

  // Trace retained objects
  Allocation* cur = objects;
  while (cur)
  {
    if (cur->reference_count > 0 && cur->size >= 0) ((Object*)cur)->trace();
    cur = cur->next_allocation;
  }

  // Keep or destroy each object in the master list
  cur = objects;
  objects = 0;
  while (cur)
  {
    Allocation* next = cur->next_allocation;
    if (cur->size >= 0)
    {
      StringBuilder buffer;
      printf( "Deleting %s\n", ((Object*)cur)->type->name );
    }
    else
    {
      // Keep
      cur->size = ~cur->size;
      cur->next_allocation = survivors;
      survivors = cur;
    }
    cur = next;
  }

  if (objects)
  {
    // New objects have been created during GC.  Find the last survivor and
    // link it to the new objects list.
    cur = survivors;
    while (cur->next_allocation) cur = cur->next_allocation;
    cur->next_allocation = objects;
  }
  objects = (Object*) survivors;

  // Collect system garbage
  if (have_new_system_objects)
  {
    have_new_system_objects = false;
    survivors = 0;

    // Trace known system objects
    // TODO

    // Keep or destroy each object in the master list
    cur = system_objects;
    system_objects = 0;
    while (cur)
    {
      Allocation* next = cur->next_allocation;
      if (cur->size < 0)
      {
        // Keep
        cur->size = ~cur->size;
        cur->next_allocation = survivors;
        survivors = cur;
      }
      else
      {
        delete (SystemObject*) cur;
      }
      cur = next;
    }

    system_objects = (SystemObject*) survivors;
  }
}

String* VM::consolidate( const char* st )
{
  TableEntry* entry = strings->find( st );
  if (entry)
  {
    return (String*) entry->value;
  }
  else
  {
    String* result = create_string( st );
    strings->set( result, result );
    return result;
  }
}

String* VM::consolidate( Character* characters, Integer count )
{
  TableEntry* entry = strings->find( characters, count );
  if (entry)
  {
    return (String*) entry->value;
  }
  else
  {
    String* result = create_string( characters, count );
    strings->set( result, result );
    return result;
  }
}

Method* VM::create_routine( Cmd* t, String* name )
{
  name = consolidate( name );
  Method* r = new Method( t, name );
  routine_list->add( r );
  routines_by_name->set( r->name, r );
  return r;
}

Logical VM::execute( const char* utf8 )
{
  if (utf8)
  {
    Parser parser( this );
    if ( !parser.parse("Program Input",utf8) ) return false;
  }

  BOSS_TRY(this)
  {
    Scope scope(this);
    for (int i=routine_list->count; --i>=0; )
    {
      ((Method*)routine_list->at(i))->resolve( &scope );
    }
    main_routine->statements.execute();
    main_routine->statements.clear();
    main_routine->resolved = false;
    return true;
  }
  BOSS_CATCH_ANY
  {
    log_error();
    return false;
  }
  BOSS_END_TRY(this)
}

Method* VM::get_routine( Cmd* t, const char* name )
{
  /*
  TableEntry* entry = routines->find( name );
  if (entry) return (Method*) entry->value;

  Method* r = new Method( t, consolidate(name) );
  routines->set( name, r );
  return r;
  */
  return 0;
}

Array* VM::create_array( Type* array_type, Integer count )
{
  Array* array = (Array*) array_type->create_object( sizeof(Array) + array_type->element_size()*count );
  array->count = count;
  return array;
}

List* VM::create_list( Type* list_type, Integer initial_capacity )
{
  List* list = (List*) list_type->create_object();
  list->array = create_array( list_type->array_type(), initial_capacity );
  return list;
}

String* VM::create_string( const char* utf8_data, Integer utf8_count )
{
  if (utf8_count == -1) utf8_count = strlen( utf8_data );

  int decoded_count = UTF8::decoded_count( utf8_data, utf8_count );
  String* st = create_string( decoded_count );

  UTF8::decode( utf8_data, utf8_count, st->characters, decoded_count );

  return st->update_hash_code();
}

String* VM::create_string( Character* data, Integer count )
{
  String* st = create_string( count );

  memcpy( st->characters, data, count*sizeof(Character) );

  return st->update_hash_code();
}

String* VM::create_string( Integer character_count )
{
  int obj_size = sizeof(String) + (character_count * sizeof(Character));
  String* st = (String*) type_String->create_object( obj_size );
  st->count = character_count;
  return st;
}

Table* VM::create_table( Integer bin_count )
{
  Table* table = (Table*) type_Table->create_object();
  table->init( bin_count );
  return table;
}

Logical VM::load( const char* filepath )
{
  Parser parser( this );
  if ( !parser.parse(filepath) ) return false;
  return true;
}

void VM::log_error()
{
  StringBuilder buffer;
  buffer.print( "ERROR" );
  if (error_filepath)
  {
    buffer.print( " [" ).print( error_filepath );
    if (error_line)
    {
      buffer.print( " line " ).print( error_line );
    }
  }
  buffer.print( "]: " ).print( error_message ).println();
  buffer.log();
}


}; // namespace Boss
