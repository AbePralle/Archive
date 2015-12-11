package alphavm;

import java.util.*;

public class BardList extends BardObject
{
  public BardObject[] data;
  public int          count;
  public int          write_pos;

  public BardList( BardType type, int capacity )
  {
    super( type );
    init( capacity );
  }

  public BardList init( int capacity )
  {
    if (capacity > 0 && (data == null || capacity > data.length))
    {
      data = BardMM.replace_Object_array( data, capacity );
    }
    return this;
  }

  public BardObject reset()
  {
    count = 0;
    write_pos = 0;
    return this;
  }

  public BardObject collect_references( BardObject list )
  {
    list = super.collect_references( list );

    BardObject[] data = this.data;
    int i = count;
    while (--i >= 0)
    {
      BardObject obj = data[i];
      if (obj != null && !obj.referenced) 
      {
        obj.referenced = true;
        obj.next_object = list;
        list = obj;
      }
    }

    return list;
  }

  public void clear()
  {
    int pos = count;
    while (--pos >= 0)
    {
      data[pos] = null;
    }
    count = 0;
    write_pos = 0;
  }

  public int capacity()
  {
    if (data == null) return 0;
    return data.length;
  }

  public void add( BardObject obj )
  {
    if (data == null) ensure_capacity( 10 );
    else if (count == data.length) ensure_capacity( count * 2 );

    data[count++] = obj;
  }

  public void insert( BardObject obj, int before_index )
  {
    if (before_index == count)
    {
      add( obj );
    }
    else
    {
      int shift_count = count - before_index;
      add( null );  // note: count changes here
      System.arraycopy( data, before_index, data, before_index+1, shift_count );
      data[ before_index ] = obj;
    }
  }

  public void reverse()
  {
    int i1 = 0;
    int i2 = count - 1;
    while (i1 < i2)
    {
      BardObject temp = data[i1];
      data[i1] = data[i2];
      data[i2] = temp;
      ++i1;
      --i2;
    }
  }

  public void save( BardObject obj )
  {
    if (write_pos == count) add( obj );
    else                    data[write_pos] = obj;
    ++write_pos;
  }

  public void load()
  {
    while (count > write_pos)
    {
      data[--count] = null;
    }
    write_pos = 0;
  }

  public BardObject get( int index )
  {
    return data[ index ];
  }

  public void set( int index, BardObject value )
  {
    data[index] = value;
  }

  public BardObject remove_at( int index )
  {
    BardObject result = data[index];
    --count;

    if (index < count)
    {
      System.arraycopy( data, index+1, data, index, count - index );
    }

    data[count] = null;

    return result;
  }

  public boolean remove_Object( BardObject value )
  {
    if (value == null)
    {
      for (int i=0; i<count; ++i)
      {
        if (data[i] == null)
        {
          remove_at(i);
          return true;
        }
      }
      return false;
    }
    else
    {
      for (int i=0; i<count; ++i)
      {
        if (value == data[i])
        {
          remove_at(i);
          return true;
        }
      }

      for (int i=0; i<count; ++i)
      {
        if (value.equals(data[i]))
        {
          remove_at(i);
          return true;
        }
      }
      return false;
    }
  }

  public boolean remove_Real( double value )
  {
    for (int i=0; i<count; ++i)
    {
      BardObject obj = data[i];
      if (obj != null && obj instanceof BardValue.RealValue && ((BardValue.RealValue)obj).value == value)
      {
        remove_at(i);
        return true;
      }
    }
    return false;
  }

  public boolean remove_Integer( int value )
  {
    for (int i=0; i<count; ++i)
    {
      BardObject obj = data[i];
      if (obj != null && obj instanceof BardValue.IntegerValue && ((BardValue.IntegerValue)obj).value == value)
      {
        remove_at(i);
        return true;
      }
    }
    return false;
  }

  public boolean remove_Character( int value )
  {
    for (int i=0; i<count; ++i)
    {
      BardObject obj = data[i];
      if (obj != null && obj instanceof BardValue.CharacterValue && ((BardValue.CharacterValue)obj).value == value)
      {
        remove_at(i);
        return true;
      }
    }
    return false;
  }

  public boolean remove_Logical( int value )
  {
    for (int i=0; i<count; ++i)
    {
      BardObject obj = data[i];
      if (obj != null && obj instanceof BardValue.LogicalValue && ((BardValue.LogicalValue)obj).value == value)
      {
        remove_at(i);
        return true;
      }
    }
    return false;
  }

  public void ensure_capacity( int capacity )
  {
    if (data == null)
    {
      data = BardMM.acquire_Object_array( capacity );
      return;
    }

    if (data.length >= capacity) return;
    data = BardMM.resize_Object_array( data, count, capacity );
  }

  public String toString()
  {
    StringBuilder buffer = new StringBuilder();
    buffer.append('[');

    boolean first = true;
    for (int i=0; i<count; ++i)
    {
      BardObject obj = data[i];

      if (first) first = false;
      else       buffer.append(',');

      buffer.append( obj );
    }

    buffer.append(']');
    return buffer.toString();
  }
}
