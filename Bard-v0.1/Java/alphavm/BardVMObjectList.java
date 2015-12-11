package alphavm;

public class BardVMObjectList
{
  public BardObject[] data;
  public int          count;
  public int          write_pos;

  public BardVMObjectList()
  {
    this( 256 );
  }

  public BardVMObjectList( int capacity )
  {
    if (capacity > 0) data = new BardObject[ capacity ];
  }

  public BardObject[] to_array()
  {
    BardObject[] array = new BardObject[ count ];
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

  public void add( BardObject n )
  {
    if (data == null) ensure_capacity( 10 );
    else if (count == data.length) ensure_capacity( count * 2 );
    data[count++] = n;
  }

  public void save( BardObject n )
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
    for (int i=index; i<count; ++i) data[i] = data[i+1];
    return result;
  }

  public void ensure_capacity( int capacity )
  {
    if (data == null)
    {
      data = new BardObject[ capacity ];
      return;
    }

    if (data.length >= capacity) return;

    BardObject[] new_data = new BardObject[ capacity ];
    System.arraycopy( data, 0, new_data, 0, count );
    data = new_data;
  }
}

