package boss.vm;

public class BossReal64List
{
  public double[] data;
  public int      count;

  public BossReal64List()
  {
  }

  public BossReal64List( int initialCapacity )
  {
    if (initialCapacity > 0) data = new double[ initialCapacity ];
  }

  public void add( double value )
  {
    reserve( 1 );
    data[ count++ ] = value;
  }

  public void clear()
  {
    count = 0;
  }

  public boolean contains( double value )
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
      double[] data = this.data;
      for (int i=minimumCount; --i>=count; )
      {
        data[ i ] = 0;
      }
      this.count = minimumCount;
    }
  }

  public double first()
  {
    return data[ 0 ];
  }

  public double get( int index )
  {
    return data[ index ];
  }

  public void insert( double value, int atIndex )
  {
    reserve( 1 );
    double[] data = this.data;
    for (int i=count; --i>=atIndex; )
    {
      data[i+1] = data[i];
    }
    data[ atIndex ] = value;
    ++count;
  }

  public double last()
  {
    return data[ count-1 ];
  }

  public int locate( double value )
  {
    int n = count;
    for (int i=0; i<count; ++i)
    {
      if (data[i] == value) return i;
    }
    return -1;
  }

  public boolean remove( double value )
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

  public double removeAt( int index )
  {
    double result = data[index];
    int n = --count;
    for (int i=index; i<n; ++i)
    {
      data[i] = data[i+1];
    }
    return result;
  }

  public double removeLast()
  {
    --count;
    return data[ count ];
  }

  public void reserve( int additionalElementCount )
  {
    if (additionalElementCount <= 0) return;
    if (data == null)
    {
      if (additionalElementCount < 10) additionalElementCount = 10;
      data = new double[ additionalElementCount ];
    }
    else
    {
      int requiredCapacity = count + additionalElementCount;
      int currentCapacity = data.length;
      if (requiredCapacity > currentCapacity)
      {
        int doubleCapacity = currentCapacity * 2;
        if (doubleCapacity > requiredCapacity) requiredCapacity = doubleCapacity;
        double[] newData = new double[ requiredCapacity ];
        double[] data = this.data;
        for (int i=count; --i>=0; )
        {
          newData[i] = data[i];
        }
        this.data = newData;
      }
    }
  }

  public void set( int index, double value )
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
