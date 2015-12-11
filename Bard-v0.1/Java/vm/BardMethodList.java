package vm;

import java.util.*;

public class BardMethodList
{
  public BardMethod[] data;
  public int          count;
  public int          write_pos;

  public BardMethodList()
  {
    this( 0 );
  }

  public BardMethodList( int capacity )
  {
    if (capacity > 0) data = new BardMethod[ capacity ];
  }

  public BardMethod[] to_array()
  {
    BardMethod[] array = new BardMethod[ count ];
    System.arraycopy( data, 0, array, 0, count );
    return array;
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

  public void add( BardMethod n )
  {
    if (data == null) ensure_capacity( 10 );
    else if (count == data.length) ensure_capacity( count * 2 );

    data[count++] = n;
  }

  public void save( BardMethod n )
  {
    if (write_pos == count) add( n );
    else                    data[write_pos] = n;
    ++write_pos;
  }

  public void load()
  {
    count = write_pos;
    write_pos = 0;
  }

  public BardMethod get( int index )
  {
    return data[ index ];
  }

  public void set( int index, BardMethod value )
  {
    data[index] = value;
  }

  public void ensure_capacity( int capacity )
  {
    if (data == null)
    {
      data = new BardMethod[ capacity ];
      return;
    }

    if (data.length >= capacity) return;

    BardMethod[] new_data = new BardMethod[ capacity ];
    System.arraycopy( data, 0, new_data, 0, count );
    data = new_data;
  }

  public int index_of( int method_id )
  {
    for (int i=count; --i>=0; )
    {
      BardMethod m = get(i);
      if (m.method_id == method_id) return i;
    }
    return -1;
  }

  public BardMethod find( int method_id )
  {
    for (int i=count-1; i>=0; --i )
    {
      BardMethod m = data[i];
      if (m.method_id == method_id) return m;
    }
    return null;
  }

  public String toString()
  {
    StringBuilder buffer = new StringBuilder();
    buffer.append('{');
    boolean first = true;
    for (int i=0; i<count; ++i)
    {
      BardMethod m = data[i];
      if (first) first = false;
      else       buffer.append( ',' ); 
      buffer.append( m.signature ); 
    }
    buffer.append('}');
    return buffer.toString();
  }
}

