package boss.vm;

public class BossStringIntLookup
{
  // Maps String -> int
  public BossStringList keys     = new BossStringList( 256 );
  public BossIntList    values   = new BossIntList( 256 );
  public BossStringList ordering = new BossStringList( 256 );

  public int add( String key )
  {
    int value = locate( key );
    if (value != -1) return values.get( value );

    value = keys.count;
    set( key, value );
    return value;
  }

  public int add( BossStringBuilder key )
  {
    int value = locate( key );
    if (value != -1) return values.get( value );

    value = keys.count;
    set( key.toString(), value );
    return value;
  }

  public int count()
  {
    return keys.count;
  }

  public String get( int index )
  {
    return ordering.get( index );
  }

  public int get( String key )
  {
    return add( key );
  }

  public int get( BossStringBuilder key )
  {
    return add( key );
  }

  public int locate( String key )
  {
    return locate( key, 0, keys.count-1 );
  }

  public int locate( String key, int min, int max )
  {
    if (min > max) return -1;
    int mid = (min + max) / 2;

    String keyAtMid = keys.get( mid );
    int delta = key.compareTo( keyAtMid );
    if (delta == 0) return mid;
    if (delta < 0)  return locate( key, min, mid-1 );
    else            return locate( key, mid+1, max );
  }

  public int locate( BossStringBuilder key )
  {
    return locate( key, 0, keys.count-1 );
  }

  public int locate( BossStringBuilder key, int min, int max )
  {
    if (min > max) return -1;
    int mid = (min + max) / 2;

    String keyAtMid = keys.get( mid );
    int delta = key.compareTo( keyAtMid );
    if (delta == 0) return mid;
    if (delta < 0)  return locate( key, min, mid-1 );
    else            return locate( key, mid+1, max );
  }

  public void set( String key, int value )
  {
    int index = locate( key );
    if (index != -1)
    {
      values.set( index, value );
      return;
    }

    ordering.add( key );

    if (keys.count == 0 || key.compareTo(keys.last()) > 0)
    {
      keys.add( key );
      values.add( value );
    }
    else if (key.compareTo(keys.first()) < 0)
    {
      keys.insert( key, 0 );
      values.insert( value, 0 );
    }
    else
    {
      keys.add( null );
      values.add( 0 );
      for (int i=keys.count-1; --i>=0; )
      {
        String shiftKey = keys.get( i );
        if (key.compareTo(shiftKey) < 0)
        {
          keys.set( i+1, shiftKey );
          values.set( i+1, values.get(i) );
        }
        else
        {
          keys.set( i+1, key );
          values.set( i+1, value );
          break;
        }
      }
    }
  }

  public String toString()
  {
    StringBuilder builder = new StringBuilder();
    builder.append( '[' );
    for (int i=0; i<ordering.count; ++i)
    {
      if (i > 0) builder.append( ',' );
      String key = ordering.get(i);
      builder.append( key ).append( ':' );
      builder.append( get(key) );
    }
    builder.append( ']' );
    return builder.toString();
  }
};
