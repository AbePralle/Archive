package vm;

import java.util.*;

public class BardByteList extends BardObject
{
  public byte[] data;
  public int    count;
  public int    write_pos;

  public BardByteList( BardType type, int capacity )
  {
    super( type );
    if (capacity > 0) data = new byte[ capacity ];
  }

  public void clear()
  {
    count = 0;
    write_pos = 0;
  }

  public int capacity()
  {
    if (data == null) return 0;
    return data.length;
  }

  public void add( int n )
  {
    if (data == null) ensure_capacity( 10 );
    else if (count == data.length) ensure_capacity( count * 2 );

    data[count++] = (byte)n;
  }

  public void insert( int n, int before_index )
  {
    if (before_index == count)
    {
      add( n );
    }
    else
    {
      int shift_count = count - before_index;
      add( 0 );  // note: count changes here
      System.arraycopy( data, before_index, data, before_index+1, shift_count );
      data[ before_index ] = (byte)n;
    }
  }

  public void save( int n )
  {
    if (write_pos == count) add( n );
    else                    data[write_pos] = (byte) n;
    ++write_pos;
  }

  public void load()
  {
    count = write_pos;
    write_pos = 0;
  }

  public int get( int index )
  {
    return ((int)data[ index ]) & 255;
  }

  public void set( int index, int value )
  {
    data[index] = (byte) value;
  }

  public int remove_at( int index )
  {
    int result = ((int)data[index]) & 255;
    --count;

    if (index < count)
    {
      System.arraycopy( data, index+1, data, index, count - index );
    }

    return result;
  }

  public void ensure_capacity( int capacity )
  {
    if (data == null)
    {
      data = new byte[ capacity ];
      return;
    }

    if (data.length >= capacity) return;

    byte[] new_data = new byte[ capacity ];
    System.arraycopy( data, 0, new_data, 0, count );
    data = new_data;
  }

  public String toString()
  {
    StringBuilder buffer = new StringBuilder();
    buffer.append('[');

    boolean first = true;
    for (int i=0; i<count; ++i)
    {
      int n = data[i];

      if (first) first = false;
      else       buffer.append(',');

      buffer.append( n&255 );
    }

    buffer.append(']');
    return buffer.toString();
  }
}
