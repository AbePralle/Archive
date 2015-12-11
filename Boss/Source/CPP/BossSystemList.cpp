//=============================================================================
//  BossSystemList.cpp
//
//  2010.08.27 by Abe Pralle
//=============================================================================
#include "Boss.h"
using namespace Boss;

SystemList::SystemList( Integer capacity ) : count(0)
{
  this->capacity = capacity;
  data = new void*[capacity];
  memset( data, 0, sizeof(void*)*capacity );
  count = 0;
}

SystemList::~SystemList()
{
  delete data;
  data = 0;
  count = 0;
  capacity = 0;
}

SystemList* SystemList::add( void* value )
{
  if (count == capacity) ensure_capacity( capacity ? capacity*2 : 10 );
  data[count++] = value;
  return this;
}

SystemList* SystemList::clear() { count = 0; return this; }

SystemList* SystemList::discard( Integer i1, Integer i2 )
{
  if (i1 < 0)      i1 = 0;
  if (i2 >= count) i2 = count - 1;

  if (i1 > i2) return this;

  if (i2 == count-1)
  {
    if (i1 == 0) clear();
    else         count = i1;
    return this;
  }

  memmove( data+i1, data+i2+1, (count-(i2+1)) * sizeof(void*) );
  count -= (i2-i1) + 1;
  return this;
}

SystemList* SystemList::discard_from( Integer i1 )
{
  return discard( i1, count-1 );
}

void SystemList::remove( void* value )
{
  for (Integer i=count-1; i>=0; --i)
  {
    if (data[i] == value)
    {
      remove_at(i);
      return;
    }
  }
}

void* SystemList::remove_at( Integer index )
{
  if (index == count-1)
  {
    return data[--count];
  }
  else
  {
    void* result = data[index];
    --count;
    while (index < count)
    {
      data[index] = data[index+1];
      ++index;
    }
    return result;
  }
}

void* SystemList::remove_last()
{
  return data[ --count ];
}

SystemList* SystemList::reserve( Integer additional_count )
{
  return ensure_capacity( count + additional_count );
}

SystemList* SystemList::ensure_capacity( Integer c )
{
  if (capacity >= c) return this;

  if (capacity > 0)
  {
    Integer double_capacity = (capacity << 1);
    if (double_capacity > c) c = double_capacity;
  }

  capacity = c;

  Integer bytes = sizeof(void*) * capacity;

  if ( !data )
  {
    data = new void*[capacity];
    memset( data, 0, sizeof(void*) * capacity );
  }
  else
  {
    Integer old_bytes = sizeof(void*) * count;

    void** new_data = new void*[capacity];
    memset( ((char*)new_data) + old_bytes, 0, bytes - old_bytes );
    memcpy( new_data, data, old_bytes );

    delete data;
    data = new_data;
  }

  return this;
}
