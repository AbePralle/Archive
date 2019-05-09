package boss.vm;

import java.util.ArrayList;

public class BossStringIntTable
{
  final static public int
    HASH_CODE_OFFSET  = 0,
    KEY_INDEX_OFFSET  = 1,
    VALUE_OFFSET      = 2,
    BIN_LINK_OFFSET   = 3,

    ENTRY_SIZE        = 4;

  public int[]             bins;
  public ArrayList<String> keys;
  public BossInt32List     data;     // [hashcode, key index, value, link]; entry 0 = null
  public BossInt32List     entries;  // ordered list of entries

  public int freeList;
  public int count;
  public int binCount;

  public BossStringIntTable()
  {
    this( 256 );
  }

  public BossStringIntTable( int binCount )
  {
    int pow2 = 1;
    while (pow2 < binCount) pow2 = pow2 << 1;
    binCount = pow2;

    this.binCount = binCount;

    // The initial capacities of the keys and data lists don't need to be
    // tied to binCount but it seems like a reasonable correlation.
    bins = new int[ binCount ];
    keys = new ArrayList<String>( binCount );
    data = new BossInt32List( binCount*ENTRY_SIZE );
    entries = new BossInt32List( binCount );

    for (int i=ENTRY_SIZE; --i>=0; ) data.add( 0 );
  }

  public int add( String key )
  {
    int entry = find( key );
    if (entry != 0) return data.get( entry+VALUE_OFFSET );

    int value = count;
    set( key, value );
    return value;
  }

  public int add( BossStringBuilder key )
  {
    int entry = find( key );
    if (entry != 0) return data.get( entry+VALUE_OFFSET );

    int value = count;
    set( key.toString(), value );
    return value;
  }

  public void clear()
  {
    for (int i=0; i<binCount; ++i) bins[i] = 0;
    keys.clear();
    data.clear();
    entries.clear();
    freeList = count = 0;
    for (int i=ENTRY_SIZE; --i>=0; ) data.add( 0 );
  }

  public boolean contains( String key )
  {
    return find(key) != 0;
  }

  public int find( String key )
  {
    return find( key, hashCode(key) );
  }

  public int find( String key, int hash )
  {
    int bin   = hash & (binCount-1);
    int entry = bins[ bin ];
    while (entry > 0)
    {
      if (data.get(entry+HASH_CODE_OFFSET) == hash && key.equals(keys.get(data.get(entry+KEY_INDEX_OFFSET)))) return entry;
      entry = data.get( entry + BIN_LINK_OFFSET );
    }
    return 0;
  }

  public int find( BossStringBuilder key )
  {
    return find( key, key.hashCode() );
  }

  public int find( BossStringBuilder key, int hash )
  {
    int bin   = hash & (binCount-1);
    int entry = bins[ bin ];
    while (entry > 0)
    {
      if (data.get(entry+HASH_CODE_OFFSET) == hash && key.equals(keys.get(data.get(entry+KEY_INDEX_OFFSET)))) return entry;
      entry = data.get( entry + BIN_LINK_OFFSET );
    }
    return 0;
  }

  public int get( String key )
  {
    int entry = find( key );
    if (entry == 0) return 0;
    return data.get( entry+VALUE_OFFSET );
  }

  public int hashCode( String value )
  {
    // Compute the same hash as BossStringBuilder()
    int n = value.length();
    int hash = 0;
    for (int i=0; i<n; ++i)
    {
      hash = ((hash << 3) - hash) + value.charAt( i );
    }
    return hash;
  }

  public String keyAt( int index )
  {
    return keys.get( data.get(entries.get(index)+KEY_INDEX_OFFSET) );
  }

  public int remove( String key )
  {
    int entry = find( key );
    if (entry == 0) return 0;

    if (count == 1)
    {
      int result = data.get( entry+VALUE_OFFSET );
      clear();
      return result;
    }

    int hash = data.get( entry+HASH_CODE_OFFSET );
    int bin = hash & (binCount-1);
    int curEntry = bins[ bin ];
    if (curEntry == entry)
    {
      bins[ bin ] = data.get( entry + BIN_LINK_OFFSET );
    }
    else
    {
      int nextEntry = data.get( curEntry+BIN_LINK_OFFSET );
      while (nextEntry != entry)
      {
        curEntry = nextEntry;
        nextEntry = data.get( curEntry+BIN_LINK_OFFSET );
      }
      data.set( curEntry+BIN_LINK_OFFSET, data.get(nextEntry+BIN_LINK_OFFSET) );  // cur = next.link
    }

    int keySlot = data.get( entry+KEY_INDEX_OFFSET );
    keys.set( keySlot, null );
    data.set( entry+BIN_LINK_OFFSET, freeList );
    freeList = entry;

    entries.remove( entry );

    --count;
    return data.get( entry+VALUE_OFFSET );
  }

  public int set( String key, int value )
  {
    // Returns the index of the entry
    int hash = hashCode( key );

    int existing = find( key, hash );
    if (existing > 0)
    {
      // Replace existing entry
      data.set( existing+VALUE_OFFSET, value );
      return existing;
    }
    else
    {
      ++count;
      int bin = hash & (binCount - 1);
      int result;
      if (freeList > 0)
      {
        result = freeList;
        freeList = data.get( freeList + BIN_LINK_OFFSET );

        int keySlot = data.get( result + KEY_INDEX_OFFSET );
        keys.set( keySlot, key );
        data.set( result+HASH_CODE_OFFSET,  hash );
        data.set( result+VALUE_OFFSET,      value );
        data.set( result+BIN_LINK_OFFSET,   bins[bin] );
      }
      else
      {
        result = data.count;
        data.add( hash );
        data.add( keys.size() );
        data.add( value );
        data.add( bins[bin] );
        keys.add( key );
      }

      entries.add( result );

      bins[ bin ] = result;
      return result;
    }
  }

  public String toString()
  {
    BossStringBuilder builder = new BossStringBuilder();
    builder.print( '{' );
    for (int i=0; i<entries.count; ++i)
    {
      if (i > 0) builder.print( ',' );
      int cur = entries.get( i );
      builder.print( keys.get(data.get(cur+KEY_INDEX_OFFSET)) );
      builder.print( ':' );
      builder.print( data.get(cur+VALUE_OFFSET) );
    }
    builder.print( '}' );
    return builder.toString();
  }
};
