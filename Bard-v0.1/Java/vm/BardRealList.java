package vm;

public class BardRealList
{
  public double[] data;
  public int      count;
  public int      write_pos;

  public BardRealList()
  {
    this( 0 );
  }

  public BardRealList( int capacity )
  {
    if (capacity > 0) data = new double[ capacity ];
  }

  public double[] to_array()
  {
    double[] array = new double[ count ];
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

  public void add( double n )
  {
    if (data == null) ensure_capacity( 10 );
    else if (count == data.length) ensure_capacity( count * 2 );

    data[count++] = n;
  }

  public void save( double n )
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

  public double get( int index )
  {
    return data[ index ];
  }

  public void set( int index, double value )
  {
    data[index] = value;
  }

  public void ensure_capacity( int capacity )
  {
    if (data == null)
    {
      data = new double[ capacity ];
      return;
    }

    if (data.length >= capacity) return;

    double[] new_data = new double[ capacity ];
    System.arraycopy( data, 0, new_data, 0, count );
    data = new_data;
  }
}

