package alphavm;

import java.util.*;

public class BardCharacterList extends BardString
{
  public BardCharacterList( BardType type, int capacity )
  {
    super( type, capacity );
  }

  public BardCharacterList reset()
  {
    clear();
    return this;
  }

  public void clear()
  {
    count = 0;
    write_pos = 0;
    hash_code = 0;
  }

  public int capacity()
  {
    if (data == null) return 0;
    return data.length;
  }

  public void add( int n )
  {
    if (data == null) ensure_capacity( 40 );
    else if (count == data.length) ensure_capacity( count * 2 );

    data[count++] = (char)n;
    hash_code = ((hash_code<<3) - hash_code) + n;   // code = code * 7 + n
  }

  public void print( Object obj )
  {
    print( obj.toString() );
  }

  public void print( String st )
  {
    int count = st.length();
    for (int i=0; i<count; ++i)
    {
      add( st.charAt(i) );
    }
  }

  public void print( double n )
  {
    if (n < 0)
    {
      print( '-' );
      n = -n;
    }

    double floor = Math.floor( n );
    print( (long) floor );
    print( '.' );

    long rem = (long) Math.round((n - floor) * 10000.0);
    if (rem < 1000) print( '0' );
    if (rem < 100) print( '0' );
    if (rem < 10) print( '0' );
    print( rem );
  }

  public void print( long n )
  {
    if (n < 0)
    {
      add( '-' );
      if (n == -n)
      {
        // Special case - happens with the max negative number.
        print( -(n/10) );
        print( -(n%10) );
      }
      else
      {
        print( -n );
      }
    }
    else if (n == 0)
    {
      add( '0' );
    }
    else
    {
      int i1 = count;
      while (n > 0)
      {
        add( (char) ((n%10)+'0') );
        n /= 10;
      }
      int i2 = count - 1;

      // Reverse printed subset
      while (i1 < i2)
      {
        char temp = data[i1];
        data[i1] = data[i2];
        data[i2] = temp;
        ++i1;
        --i2;
      }
    }
  }

  public void print( char n )
  {
    add( n );
  }

  public void print( boolean n )
  {
    if (n) print( "true" );
    else   print( "false" );
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
      data[ before_index ] = (char)n;
    }
  }

  public void save( int n )
  {
    if (write_pos == count) add( n );
    else                    data[write_pos] = (char) n;
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
    data[index] = (char) value;
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
      data = new char[ capacity ];
      return;
    }

    if (data.length >= capacity) return;

    char[] new_data = new char[ capacity ];
    System.arraycopy( data, 0, new_data, 0, count );
    data = new_data;
  }
}

