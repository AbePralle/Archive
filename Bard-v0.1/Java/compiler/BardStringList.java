package compiler;

import alphavm.BardOp;

public class BardStringList
{
  public String[] data;
  public int      count;
  public int      write_pos;

  public BardStringList()
  {
    this( 0 );
  }

  public BardStringList( int capacity )
  {
    if (capacity > 0) data = new String[ capacity ];
  }

  public String[] to_array()
  {
    String[] array = new String[ count ];
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

  public void add( String n )
  {
    if (data == null) ensure_capacity( 10 );
    else if (count == data.length) ensure_capacity( count * 2 );

    data[count++] = n;
  }

  public void save( String n )
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

  public String get( int index )
  {
    return data[ index ];
  }

  public void set( int index, String value )
  {
    data[index] = value;
  }

  public String remove_at( int index )
  {
    String result = data[index];
    --count;
    for (int i=index; i<count; ++i) data[i] = data[i+1];
    return result;
  }

  public void ensure_capacity( int capacity )
  {
    if (data == null)
    {
      data = new String[ capacity ];
      return;
    }

    if (data.length >= capacity) return;

    String[] new_data = new String[ capacity ];
    System.arraycopy( data, 0, new_data, 0, count );
    data = new_data;
  }
}

