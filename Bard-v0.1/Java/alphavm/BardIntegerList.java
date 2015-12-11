package alphavm;

public class BardIntegerList extends BardObject
{
  public int[]   data;
  public int     count;
  public int     write_pos;

  public BardIntegerList()
  {
    this( 0 );
  }

  public BardIntegerList( int capacity )
  {
    this( null, capacity );
  }

  public BardIntegerList( BardType type, int capacity )
  {
    super( type );
    if (capacity > 0) data = new int[ capacity ];
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

    data[count++] = n;
  }

  public void save( int n )
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

  public int get( int index )
  {
    return data[ index ];
  }

  public int remove_at( int index )
  {
    int result = data[index];
    --count;
    for (int i=index; i<count; ++i) data[i] = data[i+1];
    return result;
  }

  public void set( int index, int value )
  {
    data[index] = value;
  }

  public void ensure_capacity( int capacity )
  {
    if (data == null)
    {
      data = new int[ capacity ];
      return;
    }

    if (data.length >= capacity) return;

    int[] new_data = new int[ capacity ];
    System.arraycopy( data, 0, new_data, 0, count );
    data = new_data;
  }
}

