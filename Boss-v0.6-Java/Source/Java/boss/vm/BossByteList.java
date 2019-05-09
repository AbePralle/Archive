package boss.vm;

public class BossByteList
{
  public byte[] data;
  public int    count;

  public BossByteList()
  {
  }

  public BossByteList( int initialCapacity )
  {
    if (initialCapacity > 0) data = new byte[ initialCapacity ];
  }

  public void add( byte value )
  {
    reserve( 1 );
    data[ count++ ] = value;
  }

  public void clear()
  {
    count = 0;
  }

  public boolean contains( byte value )
  {
    return -1 != locate( value );
  }

  public void ensureCapacity( int minimumCapacity )
  {
    reserve( minimumCapacity - count );
  }

  public void expand( int additionalElementCount )
  {
    expandToCount( count + additionalElementCount );
  }

  public void expandToCount( int minimumCount )
  {
    if (minimumCount > count)
    {
      ensureCapacity( minimumCount );
      int count = this.count;
      byte[] data = this.data;
      for (int i=minimumCount; --i>=count; )
      {
        data[ i ] = 0;
      }
      this.count = minimumCount;
    }
  }

  public byte first()
  {
    return data[ 0 ];
  }

  public byte get( int index )
  {
    return data[ index ];
  }

  public void insert( byte value, int atIndex )
  {
    reserve( 1 );
    byte[] data = this.data;
    for (int i=count; --i>=atIndex; )
    {
      data[i+1] = data[i];
    }
    data[ atIndex ] = value;
    ++count;
  }

  public byte last()
  {
    return data[ count-1 ];
  }

  public int locate( byte value )
  {
    int n = count;
    for (int i=0; i<count; ++i)
    {
      if (data[i] == value) return i;
    }
    return -1;
  }

  public boolean remove( byte value )
  {
    int n = count;
    for (int i=0; i<n; ++i)
    {
      if (data[i] == value)
      {
        removeAt( i );
        return true;
      }
    }
    return false;
  }

  public byte removeAt( int index )
  {
    byte result = data[index];
    int n = --count;
    for (int i=index; i<n; ++i)
    {
      data[i] = data[i+1];
    }
    return result;
  }

  public void reserve( int additionalElementCount )
  {
    if (additionalElementCount <= 0) return;
    if (data == null)
    {
      if (additionalElementCount < 10) additionalElementCount = 10;
      data = new byte[ additionalElementCount ];
    }
    else
    {
      int requiredCapacity = count + additionalElementCount;
      int currentCapacity = data.length;
      if (requiredCapacity > currentCapacity)
      {
        int doubleCapacity = currentCapacity * 2;
        if (doubleCapacity > requiredCapacity) requiredCapacity = doubleCapacity;
        byte[] newData = new byte[ requiredCapacity ];
        byte[] data = this.data;
        for (int i=count; --i>=0; )
        {
          newData[i] = data[i];
        }
        this.data = newData;
      }
    }
  }

  public void set( int index, byte value )
  {
    data[ index ] = value;
  }

  public String toString()
  {
    StringBuilder builder = new StringBuilder();
    builder.append( '[' );
    for (int i=0; i<count; ++i)
    {
      if (i > 0) builder.append( ',' );
      builder.append( get(i) );
    }
    builder.append( ']' );
    return builder.toString();
  }
}
