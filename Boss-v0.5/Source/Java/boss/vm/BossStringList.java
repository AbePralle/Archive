package boss.vm;

public class BossStringList
{
  public String[] data;
  public int      count;

  public BossStringList()
  {
  }

  public BossStringList( int initialCapacity )
  {
    if (initialCapacity > 0) data = new String[ initialCapacity ];
  }

  public void add( String value )
  {
    reserve( 1 );
    data[ count++ ] = value;
  }

  public void clear()
  {
    for (int i=count; --i>=0; )
    {
      data[ i ] = null;
    }
    count = 0;
  }

  public void ensureCapacity( int minimumCapacity )
  {
    reserve( minimumCapacity - count );
  }

  public void expandToCount( int minimumCount )
  {
    if (minimumCount > count)
    {
      ensureCapacity( minimumCount );
      count = minimumCount;
    }
  }

  public String first()
  {
    return data[ 0 ];
  }

  public String get( int index )
  {
    return data[ index ];
  }

  public void insert( String value, int atIndex )
  {
    reserve( 1 );
    String[] data = this.data;
    for (int i=count; --i>=atIndex; )
    {
      data[i+1] = data[i];
    }
    data[ atIndex ] = value;
    ++count;
  }

  public String last()
  {
    return data[ count-1 ];
  }

  public void reserve( int additionalElementCount )
  {
    if (additionalElementCount <= 0) return;
    if (data == null)
    {
      if (additionalElementCount < 10) additionalElementCount = 10;
      data = new String[ additionalElementCount ];
    }
    else
    {
      int requiredCapacity = count + additionalElementCount;
      int currentCapacity = data.length;
      if (requiredCapacity > currentCapacity)
      {
        int doubleCapacity = currentCapacity * 2;
        if (doubleCapacity > requiredCapacity) requiredCapacity = doubleCapacity;
        String[] newData = new String[ requiredCapacity ];
        String[] data = this.data;
        for (int i=count; --i>=0; )
        {
          newData[i] = data[i];
        }
        this.data = newData;
      }
    }
  }

  public void set( int index, String value )
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
