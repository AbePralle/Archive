//=============================================================================
//  BossType.cpp
//
//  2015.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"

namespace Boss
{

Type::Type( VM* vm, const char* name, Integer object_size, Attributes attributes )
    : vm(vm), object_size(object_size), attributes(attributes)
{
  int count = strlen( name );
  this->name = new char[ count+1 ];
  strcpy( this->name, name );
}

Object* Type::create_object( Integer object_size )
{
  if (object_size == -1) object_size = this->object_size;

  Object* obj = (Object*) vm->allocator.allocate( object_size );
  memset( obj, 0, object_size );

  obj->type = this;
  obj->size = object_size;
  obj->next_allocation = vm->objects;
  vm->objects = obj;

  return obj;
}

Type::~Type()
{
  delete name;
}

void Type::print( Object* object, StringBuilder& buffer )
{
  buffer.print( '(' ).print( name ).print( ')' );
}

void Type::trace( Object* object )
{
  if (object->size < 0) return;  // already traced
  object->size = ~object->size;
}

TypeObjectArray::TypeObjectArray( VM* vm ) : Type(vm,"ObjectArray",sizeof(Array)) {}

void TypeObjectArray::print( Object* object, StringBuilder& buffer )
{
  Array* array = (Array*) object;
  buffer.print( '[' );

  Integer  n    = array->count;
  Object** data = array->objects;
  for (int i=-1; ++i<n; )
  {
    if (i > 0) buffer.print( ',' );
    Object* obj = data[i];
    if (obj) obj->type->print( obj, buffer );
    else     buffer.print( "null" );
  }

  buffer.print( ']' );
}

void TypeObjectArray::trace( Object* object )
{
  if (object->size < 0) return;  // already traced
  object->size = ~object->size;

  Integer count     = ((Array*)object)->count;
  Object** elements = ((Array*)object)->objects - 1;
  while (--count >= 0)
  {
    Object* element;
    if ((element = *(++elements)) && element->size >= 0) element->type->trace( element );
  }
}

TypeObjectList::TypeObjectList( VM* vm ) : Type(vm,"Object[]",sizeof(List)) {}

Type* TypeObjectList::array_type()
{
  return vm->type_ObjectArray;
}

void TypeObjectList::print( Object* object, StringBuilder& buffer )
{
  List* list = (List*) object;
  buffer.print( '[' );

  Integer  n    = list->count;
  Object** data = list->array->objects;
  for (int i=-1; ++i<n; )
  {
    if (i > 0) buffer.print( ',' );
    Object* obj = data[i];
    if (obj) obj->type->print( obj, buffer );
    else     buffer.print( "null" );
  }

  buffer.print( ']' );
}

void TypeObjectList::trace( Object* object )
{
  if (object->size < 0) return;  // already traced
  object->size = ~object->size;

  ((List*)object)->array->size = ~(((List*)object)->array->size);

  Integer count     = ((List*)object)->count;
  Object** elements = ((List*)object)->array->objects - 1;
  while (--count >= 0)
  {
    Object* element;
    if ((element = *(++elements)) && element->size >= 0) element->type->trace( element );
  }
}

TypeReal::TypeReal( VM* vm ) : Type(vm,"Real",0)
{
}

TypeFloat::TypeFloat( VM* vm ) : Type(vm,"Float",0)
{
}

TypeLong::TypeLong( VM* vm ) : Type(vm,"Long",0)
{
}

TypeInteger::TypeInteger( VM* vm ) : Type(vm,"Integer",0)
{
}

TypeCharacter::TypeCharacter( VM* vm ) : Type(vm,"Character",0)
{
}

TypeByte::TypeByte( VM* vm ) : Type(vm,"Byte",0)
{
}

TypeLogical::TypeLogical( VM* vm ) : Type(vm,"Logical",0)
{
}

TypeObject::TypeObject( VM* vm ) : Type(vm,"Object",0)
{
}

TypeString::TypeString( VM* vm ) : Type(vm,"String",0)
{
}

Logical TypeString::equals( Object* a, Character* b, Integer count )
{
  Integer other_hash_code = calculate_hash_code( b, count );
  if (((String*)a)->hash_code != other_hash_code) return false;

  if (count != ((String*)a)->count) return false;

  Character* a_characters = ((String*)a)->characters;
  for (int i=count; --i>=0; )
  {
    if (a_characters[i] != b[i]) return false;
  }

  return true;
}

Logical TypeString::equals( Object* a, const char* b )
{
  Integer other_hash_code = calculate_hash_code( b );
  if (((String*)a)->hash_code != other_hash_code) return false;

  Integer other_count = strlen( b );
  if (other_count != ((String*)a)->count) return false;

  Character* a_characters = ((String*)a)->characters;
  for (int i=other_count; --i>=0; )
  {
    if (a_characters[i] != b[i]) return false;
  }

  return true;
}

Logical TypeString::equals( Object* a, Object* b )
{
  if (a == b) return true;

  if ( !b || b->type != this ) return false;

  if (((String*)a)->hash_code != ((String*)b)->hash_code) return false;

  Integer count = ((String*)a)->count;
  if (count != ((String*)b)->count) return false;

  return (0 == memcmp(((String*)a)->characters, ((String*)b)->characters, count*sizeof(Character)));
}

Integer TypeString::get_hash_code( Object* object )
{
  return ((String*)object)->hash_code;
}

void TypeString::print( Object* object, StringBuilder& buffer )
{
  String* st = ((String*) object);
  buffer.print( st->characters, st->count );
}

TypeTable::TypeTable( VM* vm ) : Type(vm,"Table",sizeof(Table)) {}

void TypeTable::print( Object* object, StringBuilder& buffer )
{
  Table* table = (Table*) object;

  buffer.print( '{' );

  Logical first = true;
  TableReader reader = table->reader();
  while (reader.has_another())
  {
    if (first) first = false;
    else       buffer.print( ',' );

    TableEntry* cur = reader.read();
    if (cur->key) cur->key->type->print( cur->key, buffer );
    buffer.print( ':' );
    if (cur->value) cur->value->type->print( cur->value, buffer );
  }

  buffer.print( '}' );
}

void TypeTable::trace( Object* object )
{
  if (object->size < 0) return;  // already traced
  object->size = ~object->size;

  ((Table*)object)->bins->size = ~(((Table*)object)->bins->size);

  TableReader reader = ((Table*)object)->reader();
  while (reader.has_another())
  {
    TableEntry* entry = reader.read();
    entry->size = ~entry->size;

    Object* key = entry->key;
    if (key) key->type->trace( key );
    
    Object* value = entry->value;
    if (value)
    {
      value->type->trace( value );
    }
  }
}

TypeTableEntry::TypeTableEntry( VM* vm ) : Type(vm,"TableEntry",sizeof(TableEntry)) {}

}; // namespace Boss
